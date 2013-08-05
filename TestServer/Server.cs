using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace TestServer
{
    public class Server
    {
        public Server(ushort listenPort)
        {
            listener = new TcpListener(new IPEndPoint(IPAddress.Loopback, listenPort));
            listener.Start();
        }

        public TestClient.Client Accept()
        {
            return new TestClient.Client(listener.AcceptTcpClient());
        }

        #region private

        private TcpListener listener;

        #endregion
    }
}
