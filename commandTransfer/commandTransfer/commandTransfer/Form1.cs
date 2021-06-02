using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;

namespace commandTransfer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private const int port = 80; // порт для прослушивания подключений
        private TcpClient client;
        private IPAddress localAddr;
        private List<KeyValuePair<int, int>> Commands = new List<KeyValuePair<int, int>>(){new KeyValuePair<int, int>(0, 0)};
        private int commandCounter;
        NetworkStream stream = null;
        private void button1_Click(object sender, EventArgs e) //start
        {
            commandCounter = 0;
            try
            {
                client = new TcpClient();
                client.Connect("192.168.0.10", 80); 
            }
            catch (SocketException ex)
            {
                richTextBox1.Text = ex.Message + "\n" + richTextBox1.Text;
            }
            catch (Exception ex)
            {
                richTextBox1.Text = ex.Message + "\n" + richTextBox1.Text;
            }
            finally
            {
                timer1.Start();
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            /*if (client == null)
            {
                richTextBox1.Text = "Сервер не подключен.\n" + richTextBox1.Text;
                timer1.Stop();
                return;
            }*/
            if (Commands.Count == 1)
            {
                richTextBox1.Text = "Данные отсутствуют.\n" + richTextBox1.Text;
                timer1.Stop();
                client.Close();
                return;
            }
            if (commandCounter >= Commands.Count - 1)
            {
                richTextBox1.Text = "Данные переданы.\n" + richTextBox1.Text;
                timer1.Stop();
                client.Close();
                return;
            }
            try
            {
                client = new TcpClient();
                client.Connect("192.168.0.10", 80);
                stream = client.GetStream();
                string response = Commands[commandCounter].Key.ToString();
                string key = Commands[commandCounter].Value.ToString();
                // преобразуем сообщение в массив байтов
                string data = '/'+response + "," + key;

                byte[] sendData = Encoding.UTF8.GetBytes(data.ToString());
                // отправка сообщения
                stream.Write(sendData, 0, sendData.Length);
                richTextBox1.Text = "Отправлено сообщение:" + data + "\n" + richTextBox1.Text;

                // Закрываем потоки
                stream.Close();
                client.Close();
                commandCounter++;
            }
            catch (SocketException ex)
            {
                richTextBox1.Text = ex.Message + "\n"  + richTextBox1.Text;
                timer1.Stop();
            }
            catch (Exception ex)
            {
                richTextBox1.Text = ex.Message + "\n" + richTextBox1.Text;
                timer1.Stop();
            }
        }

        private void button2_Click(object sender, EventArgs e) //check
        {
            try
            {
                client = new TcpClient();
                client.Connect("192.168.0.10", 80);

                NetworkStream stream = client.GetStream();

                string response = "ACS";
                byte[] data = Encoding.UTF8.GetBytes(response);
                stream.Write(data, 0, data.Length);

                // Закрываем потоки
                stream.Close();
                client.Close();
            }
            catch (SocketException ex)
            {
                richTextBox1.Text = ex.Message + "\n" + richTextBox1.Text;
            }
            catch (Exception ex)
            {
                richTextBox1.Text = ex.Message + "\n" + richTextBox1.Text;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            string[] lines = null;
            try
            {
                lines = File.ReadAllLines(@"commands.txt");
            }
            catch (Exception ex)
            {
                richTextBox1.Text = ex.Message + "\n" + richTextBox1.Text;
                return;
            }
            if (lines == null)
            {
                richTextBox1.Text = "Файл пуст. \n" + richTextBox1.Text;
                return;
            }
            foreach (string line in lines)
            {
                int[] ia = line.Split(';').Select(n => Convert.ToInt32(n)).ToArray();
                Commands.Add(new KeyValuePair<int, int>(ia[0]+5, ia[1]+314));
                richTextBox1.Text = Commands.Last().ToString() + "\n" + richTextBox1.Text;
            }

            richTextBox1.Text = "Данные загружены.\n" + richTextBox1.Text;
        }
    }
}
