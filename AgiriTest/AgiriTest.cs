using System;
using System.Diagnostics;
using System.IO;
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
        static TestServer server;

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
            server = new TestServer(10000);

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

        
        /// <summary>
        /// agiriの差込用ソケットに接続できるかどうかテスト
        /// </summary>
        [TestMethod]
        public void TestConnect()
        {
            var sock = new Socket(SocketType.Stream, ProtocolType.Tcp);
            sock.Connect("localhost", 10800);
            sock.Close();
        }

        /// <summary>
        /// 外部からsendを差し込みできるかテスト
        /// </summary>
        [TestMethod]
        public void TestInject()
        {
            var sock = new Socket(SocketType.Stream, ProtocolType.Tcp);
            sock.Connect("localhost", 10800);
            var sendData = new byte[] { 0x01, 0x00, 0x00 };
            sock.Send(sendData);
            var buffer = new byte[3];
            sock.Receive(buffer);
            Assert.AreEqual(0x01, buffer[0]);
            Assert.AreEqual(0x00, buffer[1]);
            Assert.AreEqual(0x01, buffer[2]);
            sock.Close();
        }
    }
}
