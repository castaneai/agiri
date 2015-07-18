using System;
using System.Linq;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace AgiriTest
{
    [TestClass]
    public class AgiriTest
    {
        /// <summary>
        /// クライアント側(agiri.dllを差し込む側)
        /// DLLを差し込むので，外部プロセスとして起動する
        /// </summary>
        private static Process clientProcess;

        private TcpListener testServer;

        private const UInt16 agiriPort = 10800;

        [TestInitialize]
        public void Startup()
        {
            testServer = new TcpListener(new IPEndPoint(IPAddress.Loopback, 10000));
            testServer.Start();
            var solutionDir = Path.GetFullPath(Directory.GetCurrentDirectory() + @"\..\..\..");
            var clientPath = solutionDir + @"\Debug\TestClient.exe";
            clientProcess = Process.Start(clientPath);
        }
        
        [TestCleanup]
        public void Cleanup()
        {
            clientProcess.CloseMainWindow();
            testServer.Stop();
        }
        
        /// <summary>
        /// agiriの差込用ソケットに接続できるかどうかテスト
        /// </summary>
        [TestMethod]
        public void TestConnect()
        {
            var agiriClient = new Sniffer(agiriPort);
            agiriClient.Close();
        }

        /// <summary>
        /// agiriと通信できるかテスト (ping-pong)
        /// </summary>
        [TestMethod, Timeout(3000)]
        public void TestNinjaConnection()
        {
            using (var agiriClient = new Sniffer(agiriPort)) {
                // agiriにpingを送ったらpongが返ってくるか？
                var pingRequest = new Message(Command.PingRequest);
                agiriClient.Send(pingRequest);
                var pongResponse = agiriClient.Receive();
                Assert.AreEqual(Command.PongResponse, pongResponse.Command);
                Assert.IsTrue(Enumerable.SequenceEqual(new byte[0], pongResponse.Data));
            }
        }

        /// <summary>
        /// ソケット一覧を取得できるか
        /// </summary>
        [TestMethod]
        public void TestListSocket()
        {
            using (var agiriClient = new Sniffer(agiriPort)) {
                var sockets = agiriClient.GetAllSockets();
            }
        }

        /// <summary>
        /// snifferからsendパケットを差し込むことができるか
        /// </summary>
        [TestMethod]
        public void TestInjectOutgoingPacket()
        {
            var testClientConn = testServer.AcceptTcpClient();
            using (var agiriClient = new Sniffer(agiriPort)) {
                var sockets = agiriClient.GetAllSockets().Where(si => si.EndPoint.Port == 10000).ToList();
                var targetSocket = sockets.First();
                agiriClient.InjectOutgoingPacket(targetSocket.SocketID, Encoding.ASCII.GetBytes("abcdefg\n"));
                var injectedPacketReader = new StreamReader(testClientConn.GetStream());
                var injectedPacket = injectedPacketReader.ReadLine();
                Assert.AreEqual("abcdefg", injectedPacket);
            }
        }

        /// <summary>
        /// recvパケットをsnifferに横流しできるか
        /// </summary>
        [TestMethod]
        public void TestSniffIncomingPacket()
        {
            var testClientConn = testServer.AcceptTcpClient();
            using (var sniffer = new Sniffer(agiriPort)) {
                var targetSocket = sniffer.GetAllSockets().Where(si => si.EndPoint.Port == 10000).First();
                sniffer.StartSniffIncomingPacket(targetSocket.SocketID);
                var writer = new StreamWriter(testClientConn.GetStream());
                writer.Write("SNIFFED");
                writer.Flush();
                var received = sniffer.ReceiveSniffedIncomingPacket(targetSocket.SocketID);
                Assert.AreEqual("SNIFFED", Encoding.ASCII.GetString(received));
            }
        }
    }
}
