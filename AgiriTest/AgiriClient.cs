using System;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace AgiriTest 
{
    public sealed class AgiriClient : IDisposable
    {
        private readonly TcpClient tcpClient;
        private readonly BinaryReader reader;
        private readonly BinaryWriter writer;

        public AgiriClient(ushort serverPort)
        {
            var serverEP = new IPEndPoint(IPAddress.Loopback, serverPort);
            tcpClient = new TcpClient();
            tcpClient.Connect(serverEP);
            reader = new BinaryReader(tcpClient.GetStream());
            writer = new BinaryWriter(tcpClient.GetStream());
        }

        public AgiriClient(TcpClient baseClient)
        {
            tcpClient = baseClient;
            reader = new BinaryReader(tcpClient.GetStream());
            writer = new BinaryWriter(tcpClient.GetStream());
        }

        public void Send(Message message)
        {
            writer.Write((byte)message.Command);
            writer.Write(BitConverter.GetBytes((Int32)message.Data.Length));
            writer.Write(message.Data);
            writer.Flush();
        }

        public Message Receive()
        {
            var command = (Command)reader.ReadByte();
            var length = reader.ReadInt32();
            var data = reader.ReadBytes(length);
            return new Message(command, data);
        }

        public void Close()
        {
            reader.Close();
            writer.Close();
        }

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        private void Dispose(bool disposing)
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
