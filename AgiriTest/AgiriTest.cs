using System;
using System.Diagnostics;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace AgiriTest
{
    [TestClass]
    public class AgiriTest
    {
        static TestServer.Server server;

        static Process clientProcess;

        static AgiriManager.AgiriManager manager;

        [ClassInitialize()]
        public static void Init(TestContext testContext)
        {
            // run server
            server = new TestServer.Server(10000);

            // run client
            var solutionDir = Path.GetFullPath(Directory.GetCurrentDirectory() + @"\..\..\..");
            var clientPath = solutionDir + @"\Debug\TestClient.exe";
            clientProcess = Process.Start(clientPath);

            // create agirimanager
            manager = new AgiriManager.AgiriManager(10800);
            // なぜかSleepを入れないとUdpClient.Receiveで切断される・・？
            System.Threading.Thread.Sleep(100);
        }

        [ClassCleanup]
        public static void End()
        {
            clientProcess.CloseMainWindow();
        }

        [TestMethod]
        public void TestPing()
        {
            manager.Send("ping");
            var response = manager.Receive();
            Assert.AreEqual("pong", response);
        }
    }
}
