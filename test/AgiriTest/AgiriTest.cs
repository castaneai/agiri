using System;
using System.Linq;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace AgiriTest
{
    [TestClass]
    public class AgiriTest
    {
        public TestContext TestContext { get; set; }

        /// <summary>
        /// テスト用サーバーのデフォルトの待ち受けポート番号
        /// </summary>
        private const ushort defaultTestServerListenPort = 10000;

        /// <summary>
        /// agiriのポート番号
        /// </summary>
        private const ushort agiriPort = 10800;

        /// <summary>
        /// テスト用サーバー
        /// </summary>
        private TcpListener testServer;

        /// <summary>
        /// テスト用クライアント
        /// </summary>
        private Process testClientProcess;

        /// <summary>
        /// テスト用サーバーを起動する
        /// </summary>
        /// <param name="listenPort"></param>
        /// <returns></returns>
        private static TcpListener startTestServer(ushort listenPort = defaultTestServerListenPort)
        {
            var testServer = new TcpListener(IPAddress.Loopback, listenPort);
            testServer.Start();
            return testServer;
        }

        /// <summary>
        /// テスト用クライアントを起動する
        /// </summary>
        /// <returns></returns>
        private Process startTestClientProcess()
        {
            var solutionDir = Path.GetFullPath(AppDomain.CurrentDomain.BaseDirectory + "/../../../..");
            var clientPath = solutionDir + "/Debug/test_client.exe";
            return Process.Start(clientPath, defaultTestServerListenPort.ToString());
        }

        [TestInitialize]
        public void BeforeEach()
        {
            testServer = startTestServer();
            testClientProcess = startTestClientProcess();
        }

        [TestCleanup]
        public void AfterEach()
        {
            testClientProcess.CloseMainWindow();
            testServer.Stop();
        }

        private void agiriContext(Action<StreamReader, StreamWriter, NinjaConnection> action)
        {
            using (var testClientConnection = testServer.AcceptTcpClient())
            using (var testClientReader = new StreamReader(testClientConnection.GetStream()))
            using (var testClientWriter = new StreamWriter(testClientConnection.GetStream()))
            using (var ninjaConnection = new NinjaConnection(agiriPort)) {
                action(testClientReader, testClientWriter, ninjaConnection);
            }
        }

        /// <summary>
        /// テスト用クライアントにagiri.dllを注入できたか
        /// </summary>
        [TestMethod]
        public void TestEstablishNinjaConnection()
        {
            using (var ninjaConnection = new NinjaConnection(agiriPort)) {
            }
        }

        /// <summary>
        /// agiri.dllと複数のNinjaConnectionを確立できるか
        /// </summary>
        [TestMethod]
        public void TestEstablishMultipleNinjaConnection()
        {
            using (var firstNinja = new NinjaConnection(agiriPort))
            using (var secondNinja = new NinjaConnection(agiriPort)) {
            }
        }

        /// <summary>
        /// agiri.dllと通信できるか (pingリクエストを送るとpongレスポンスが返ってくるか）
        /// </summary>
        [TestMethod]
        public void TestNinjaConnection()
        {
            using (var ninjaConnection = new NinjaConnection(agiriPort)) {
                Assert.IsTrue(ninjaConnection.Ping());
            }
        }

        /// <summary>
        /// ソケット一覧を取得できるか
        /// </summary>
        [TestMethod]
        public void TestListSocket()
        {
            agiriContext((reader, writer, ninja) => {
                var sockets = ninja.GetAllSockets();
                Assert.IsTrue(sockets.Any(s => s.EndPoint.Port == defaultTestServerListenPort), "ソケット一覧にはテストサーバーへの接続ソケットが含まれているはず");
            });
        }

        /// <summary>
        /// snifferからsendパケットを差し込むことができるか
        /// </summary>
        [TestMethod]
        public void TestInjectOutgoingPacket()
        {
            agiriContext((reader, writer, ninja) => {
                var targetSocket = ninja.GetAllSockets().First(si => si.EndPoint.Port == defaultTestServerListenPort);
                ninja.InjectOutgoingPacket(targetSocket.SocketID, Encoding.ASCII.GetBytes("abcdefg\n"));
                Assert.AreEqual("abcdefg", reader.ReadLine(), "agiriから差し込まれたメッセージがテスト用サーバーに届くはず");
            });
        }

        /// <summary>
        /// recvパケットをsnifferに横流しできるか
        /// </summary>
        [TestMethod]
        public void TestSniffIncomingPacket()
        {
            agiriContext((reader, writer, ninja) => {
                var targetSocket = ninja.GetAllSockets().First(s => s.EndPoint.Port == defaultTestServerListenPort);
                ninja.StartSniffIncomingPacket(targetSocket.SocketID);
                writer.Write("SNIFFED");
                writer.Flush();
                var received = ninja.ReceiveSniffedIncomingPacket(targetSocket.SocketID);
                Assert.AreEqual("SNIFFED", Encoding.ASCII.GetString(received), "テスト用サーバーがテスト用クライアントに返した内容をagiriが横取りできたはず");
            });
        }

    }
}
