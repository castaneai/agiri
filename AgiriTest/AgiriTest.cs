using System;
using System.Diagnostics;
using System.IO;
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
        /// サーバー側
        /// </summary>
        static TestServer.Server server;


        /// <summary>
        /// 最初に1度だけ実行される初期化
        /// </summary>
        /// <param name="testContext"></param>
        [ClassInitialize()]
        public static void Init(TestContext testContext)
        {
            // run server
            server = new TestServer.Server(10000);

            // run client
            var solutionDir = Path.GetFullPath(Directory.GetCurrentDirectory() + @"\..\..\..");
            var clientPath = solutionDir + @"\Debug\TestClient.exe";
            clientProcess = Process.Start(clientPath);
        }

        /// <summary>
        /// 最後に1度だけ実行される
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
            var testInjectorClient = new TestServer.Client(10800);
        }

        /// <summary>
        /// 外部からsendを差し込みできるかテスト
        /// </summary>
        [TestMethod]
        public void TestInject()
        {
            var client = server.Accept();

            var testInjectorClient = new TestServer.Client(10800);
            System.Threading.Thread.Sleep(1000);
            testInjectorClient.Send("agiri");

            var receivedMessage = client.Receive();
            Assert.AreEqual("agiri", receivedMessage);
        }
    }
}
