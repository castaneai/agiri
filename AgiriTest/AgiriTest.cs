using System;
using System.Linq;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
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
        static Process clientProcess;

        /// <summary>
        /// テスト用のクライアントが接続するテスト用サーバー
        /// </summary>
        static TcpListener server;

        /// <summary>
        /// テスト用サーバーの待ち受けポート番号
        /// </summary>
        private const UInt16 testServerPort = 10000;

        /// <summary>
        /// agiriの待ち受けポート番号
        /// </summary>
        private const UInt16 agiriPort = 10800;

        /// <summary>
        /// 最初に1度だけ実行される初期化
        /// テスト用クライアントを起動する
        /// （テスト用クライアントは起動後自動でagiri.dllをロードする）
        /// </summary>
        /// <param name="testContext"></param>
        [ClassInitialize()]
        public static void ClassInitialize(TestContext testContext)
        {
            // run server
            server = new TcpListener(IPAddress.Loopback, testServerPort);
            server.Start();

            // run client
            var solutionDir = Path.GetFullPath(Directory.GetCurrentDirectory() + @"\..\..\..");
            var clientPath = solutionDir + @"\Debug\TestClient.exe";
            clientProcess = Process.Start(clientPath);
        }

        /// <summary>
        /// 最後に1度だけ実行される処理
        /// テスト用クライアントを終了する
        /// </summary>
        [ClassCleanup]
        public static void ClassCleanup()
        {
            clientProcess.CloseMainWindow();
        }
        
        /// <summary>
        /// agiriの差込用ソケットに接続できるかどうかテスト
        /// </summary>
        [TestMethod]
        public void TestConnect()
        {
            var agiriClient = new AgiriClient(agiriPort);
            agiriClient.Close();
        }

        /// <summary>
        /// agiriと通信できるかテスト
        /// </summary>
        [TestMethod, Timeout(3000)]
        public void TestInject()
        {
            using (var agiriClient = new AgiriClient(agiriPort)) {
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
        [TestMethod, Timeout(3000)]
        public void TestListSocket()
        {
            using (var agiriClient = new AgiriClient(agiriPort)) {
                var request = new Message(Command.ListSocketRequest);
                agiriClient.Send(request);
                var response = agiriClient.Receive();
                Assert.AreEqual(Command.ListSocketResponse, response.Command);
            }
        }

        [TestMethod, Timeout(3000)]
        public void TestInjectSocket()
        {
            using (var agiriClient = new AgiriClient(agiriPort)) {
                // var sockets = agiriClient.GetSockets();
                // var targetSocket = sockets.Where(s => s.Port == testServerPort).First();
                // agiriClient.InjectOutgoingPacket(BitConverter.GetBytes("abcdefg\n"));
                // assert testServer.ReceiveLine() == "abcdefg"
            }
        }
    }
}
