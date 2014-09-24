using System;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace AgiriTest 
{
    public class TestClient : IDisposable
    {
        private readonly TcpClient tcpClient;

        private readonly StreamReader reader;

        private readonly StreamWriter writer;

        public TestClient(ushort serverPort)
        {
            var serverEP = new IPEndPoint(IPAddress.Loopback, serverPort);
            tcpClient = new TcpClient();
            tcpClient.Connect(serverEP);
            reader = new StreamReader(tcpClient.GetStream());
            writer = new StreamWriter(tcpClient.GetStream());
        }

        public TestClient(TcpClient baseClient)
        {
            tcpClient = baseClient;
            reader = new StreamReader(tcpClient.GetStream());
            writer = new StreamWriter(tcpClient.GetStream());
        }

        public void Send(string message)
        {
            writer.WriteLine(message);
            writer.Flush();
        }

        public string Receive()
        {
            var response = reader.ReadLine();
            return response;
        }

        public void Close()
        {
            reader.Close();
            writer.Close();
        }

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue) {
                if (disposing) {
                    reader.Dispose();
                    writer.Dispose();
                    tcpClient.Close();
                }
                disposedValue = true;
            }
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
        }
        #endregion
    }
}
