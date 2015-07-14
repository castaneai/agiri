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
        /// 最初に1度だけ実行される初期化
        /// テスト用クライアントを起動する
        /// （テスト用クライアントは起動後自動でagiri.dllをロードする）
        /// </summary>
        /// <param name="testContext"></param>
        [ClassInitialize()]
        public static void Init(TestContext testContext)
        {
            // run server
            server = new TcpListener(IPAddress.Loopback, 10000);
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
        public static void End()
        {
            clientProcess.CloseMainWindow();
        }

        public static Socket ConnectToAgiri()
        {
            var sock = new Socket(SocketType.Stream, ProtocolType.Tcp);
            sock.Connect("localhost", 10800);
            return sock;
        }
        
        /// <summary>
        /// agiriの差込用ソケットに接続できるかどうかテスト
        /// </summary>
        [TestMethod]
        public void TestConnect()
        {
            var sock = ConnectToAgiri();
            sock.Close();
        }

        /// <summary>
        /// agiriと通信できるかテスト
        /// </summary>
        [TestMethod]
        public void TestInject()
        {
            var sock = ConnectToAgiri();
            // ping を送信
            var sendData = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00 };
            sock.Send(sendData);
            // pongが返ってくるかどうか
            var buffer = new byte[] { 0x77, 0x77, 0x77, 0x77, 0x77 };
            var expected = new byte[] { 0x01, 0x00, 0x00, 0x00, 0x00 };
            sock.Receive(buffer);
            Assert.IsTrue(Enumerable.SequenceEqual(expected, buffer));
            sock.Close();
        }

        [TestMethod, Timeout(3000)]
        public void TestListSocket()
        {
            var sock = new Socket(SocketType.Stream, ProtocolType.Tcp);
            sock.Connect("localhost", 10800);
            var sendData = new byte[] { 0x03, 0x00, 0x00, 0x00, 0x00 }; // 0x03: list sokcet request
            sock.Send(sendData);
            var buffer = new byte[] { 0x77, 0x77, 0x77, 0x77, 0x77 };
            sock.Receive(buffer);
            Assert.AreEqual(0x04, buffer[0]); // 0x04 : list socket response
            Assert.AreEqual(1, BitConverter.ToInt32(buffer, 1));
        }
    }
}
