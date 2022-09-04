using MQTT_Client.comm;
using MQTT_Client.tools;
using System;
using System.Drawing;
using System.IO;
using System.Net;
using System.Windows.Forms;
using Microsoft.VisualBasic.FileIO;
using System.Text;
using OfficeOpenXml;
using System.Collections.Generic;
using MQTT_Client.Forms;
using static MQTT_Client.comm.Parser;

namespace MQTT_Client
{       
    public partial class Overview : Form
    {
        /* Global */
        const string ADMIN_PASSWORD = "147852";
        const string DEFAULT_IP = "192.168.1.132";
        const string SEPARATOR_CHAR_TOKEN = "~";

        const int DEFAULT_PORT = 1883;

        ClientInterface clientInterface;
        ManualLocation manualLocation = new ManualLocation();
        List<Cadastro> Cadastros = new List<Cadastro>();

        bool admin_mode = false;

        public Overview()
        {   
            InitializeComponent();

            if (HostStorage.LoadXML(ref manualLocation))
            {
                clientInterface = new ClientInterface("", manualLocation.IPAddress, manualLocation.Port);

                tbIPAddress.Text = manualLocation.IPAddress;
                tbName.Text = manualLocation.Name;
                tbPort.Text = manualLocation.Port.ToString();
            }
            else
            {
                clientInterface = new ClientInterface("", DEFAULT_IP, DEFAULT_PORT);
            }

            if (RegisterStorage.LoadXML(ref Cadastros) == false)
                MessageBox.Show("Falha ao carregar cadastros", "Carregamento de Cadastros");

            PopulateTopicsToLV(clientInterface.Topics);

            clientInterface.MQQTConnected += ClientInterface_MQQTConnected;
            clientInterface.MQQTDisconnected += ClientInterface_MQQTDisconnected;
            clientInterface.MQQTPublishReceived += ClientInterface_MQQTPublishReceived;
            clientInterface.MQQTSubscribed += ClientInterface_MQQTSubscribed;

            // Hide for user operation
            tabControl.TabPages.Remove(tabPageRelatorio);
            tabControl.TabPages.Remove(tabPageRegister);
            tabControl.TabPages.Remove(tabPageConfig);
        }

        private void ClientInterface_MQQTSubscribed(object sender, MQQTSubscribeEventArgs e)
        {
            return;
        }

        /// <summary>
        /// Event will be called once the topic We subscribed has data to be read
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ClientInterface_MQQTPublishReceived(object sender, MQQTPublishEventArgs e)
        {
            MESSAGE_TYPE_ID type_id = MESSAGE_TYPE_ID.UNKNOWN;
            string type_name = string.Empty, log = string.Empty;
            
            if (Parser.decode(e.Message, ref type_id, ref type_name, ref log) == false)
                return;
           
            if(lvLog.InvokeRequired)
            {
                lvLog.Invoke((MethodInvoker)delegate
                {
                    ListViewItem lvi = new ListViewItem(DateTime.Now.ToString());
                    lvi.SubItems.Add(type_name);
                    lvi.SubItems.Add(log);
                    lvLog.Items.Add(lvi);

                    lvLog.AutoResizeColumns(ColumnHeaderAutoResizeStyle.ColumnContent);
                });
            }

            if (type_id == MESSAGE_TYPE_ID.MACHINE_STATUS)
            {
                /* TODO: calling invoke with no need. fix it */
                if (lbStatusMaq.InvokeRequired)
                {
                    lbStatusMaq.Invoke((MethodInvoker)delegate
                    {
                        lbStatusMaq.Text = log;

                        if (log == "Maquina rodando")
                            lbStatusMaq.ForeColor = Color.Green;
                        else
                            lbStatusMaq.ForeColor = Color.Red;
                    });
                }

                if (log == "maintenance on")
                {
                    btnMaintenanceMode.BackColor = Color.Green;
                }
                else if (log == "maintenance off")
                {
                    btnMaintenanceMode.BackColor = SystemColors.Control;
                }
            }
        }

        private void ClientInterface_MQQTDisconnected(object sender, MQQTConnectionEventArgs e)
        {
            if (e.IsConnected)
            {
                if (btnTryConnect.InvokeRequired)
                {
                    btnTryConnect.Invoke((MethodInvoker)delegate
                    {

                        btnTryConnect.BackColor = Color.LightGreen;
                        btnTryConnect.Text = "Desconectar";
                    });
                }
            }
            else
            {
                if (btnTryConnect.InvokeRequired)
                {
                    btnTryConnect.Invoke((MethodInvoker)delegate
                    {

                        btnTryConnect.BackColor = Color.LightGray;
                        btnTryConnect.Text = "Conectar";
                    });
                }
            }
        }

        private void ClientInterface_MQQTConnected(object sender, MQQTConnectionEventArgs e)
        {
            if (e.IsConnected)
            {
                btnTryConnect.Invoke((MethodInvoker)delegate
                {

                    btnTryConnect.BackColor = Color.LightGreen;
                    btnTryConnect.Text = "Desconectar";
                });

                clientInterface.Subscribe();
            }
            else
            {

                btnTryConnect.Invoke((MethodInvoker)delegate
                {

                    btnTryConnect.BackColor = Color.LightGray;
                    btnTryConnect.Text = "Conectar";
                });

                clientInterface.Unsubscribe();
            }            
        }

        private void PopulateTopicsToLV(string[] topics)
        {
            if (topics.Length == 0)
                return;

            int i = 0;
            foreach (string topic in topics)
            {
                ListViewItem lvItem = new ListViewItem(i.ToString());
                lvItem.SubItems.Add(topic);
                lvItem.Tag = topic;
                i++;
            }
        }

        private void LvTopics_SelectedIndexChanged(object sender, EventArgs e)
        {
            clientInterface.Subscribe();
        }

        private void BtnConnect_Click(object sender, EventArgs e)
        {
            clientInterface.Connect();
        }

        private void BtnUnSubs_Click(object sender, EventArgs e)
        {
            clientInterface.Unsubscribe();
        }

        private void btnTryConnect_Click(object sender, EventArgs e)
        {
            if (clientInterface.IsConnected)
            {
                clientInterface.Disconnect();

                btnTryConnect.Invoke((MethodInvoker)delegate
                {
                    btnTryConnect.BackColor = Color.Salmon;
                    btnTryConnect.Text = "Conectar";
                });
            }
            else
            {
                if (clientInterface.Connect() == false)
                {
                    MessageBox.Show("Connection to " + clientInterface.Name +
                        Environment.NewLine + "IP: " + clientInterface.IP + " failed!");
                }
            }
        }

        private void btnSubscribe_Click(object sender, EventArgs e)
        {
            clientInterface.Subscribe();
        }

        private void btnReadBCM_23_Click(object sender, EventArgs e)
        {
            clientInterface.ReadInput(23);
        }

        private void btnReadBCM_24_Click(object sender, EventArgs e)
        {
            clientInterface.ReadInput(24);
        }

        private void paradaMaquina_click(object sender, EventArgs e)
        {
            int id_parada;
            var btn = sender as Button;
            string message = string.Empty;

            if (int.TryParse(btn.Tag.ToString(), out id_parada) == false)
                return;

            if (cbOperator.SelectedItem == null)
            {
                MessageBox.Show("Selecione um operador antes de enviar motivo de parada",
                    "Enviando Motivo de Parada", MessageBoxButtons.OK, MessageBoxIcon.Error);

                return;
            }

            message = SEPARATOR_CHAR_TOKEN + cbOperator.SelectedItem.ToString();

            if (string.IsNullOrWhiteSpace(message))
                return;

            clientInterface.SendStopReason((STOP_REASON)id_parada, message);
        }
        
        private void btnSave_Click(object sender, EventArgs e)
        {
            string failedReason = string.Empty;

            // Check all fields before creating object and save
            if (isLocationSetupValid(ref failedReason) == false)
            {
                // TODO: get text from resource to allow multi-language
                MessageBox.Show("Fields not correctly filled!" +
                    Environment.NewLine + "Reason: " + failedReason, "Edit Location");
                return;
            }

            ManualLocation ml = new ManualLocation();
            ml.IPAddress = tbIPAddress.Text;
            ml.Port = int.Parse(tbPort.Text);
            ml.Name = tbName.Text;

            HostStorage.SaveXML(ml);

            clientInterface.IP = ml.IPAddress;
            clientInterface.Port = ml.Port;
            clientInterface.Name = ml.Name;

            /* Update manual location */
            manualLocation.IPAddress = ml.IPAddress;
            manualLocation.Port = ml.Port;
            manualLocation.Name = ml.Name;
        }

        /// <summary>
        /// Check Name, IP, Command Port and Client Port
        /// </summary>
        /// <returns></returns>
        private bool isLocationSetupValid(ref string failedReason)
        {
            if (tbName.Text == string.Empty)
            {
                failedReason = "Missing location name";
                return false;
            }

            if (tbIPAddress.Text != string.Empty)
            {
                // Does it have 4 parts?
                string[] splitIPstring = tbIPAddress.Text.Split('.');
                if (splitIPstring.Length == 4)
                {
                    // Can it be transformed to a valid IP Address?
                    IPAddress stringIP;
                    if (IPAddress.TryParse(tbIPAddress.Text, out stringIP) == false)
                    {
                        failedReason = "Invalid IP Address";
                        return false;
                    }
                }
                else
                {
                    failedReason = "Invalid IP Address";
                    return false;
                }
            }

            if (tbPort.Text != string.Empty)
            {
                int port;
                if (int.TryParse(tbPort.Text, out port) == false)
                {
                    failedReason = "Invalid Command Port";
                    return false;
                }
            }

            return true;
        }
        

        private void btnReadBCM_23_Click_1(object sender, EventArgs e)
        {
            clientInterface.GetDIOStatus(23);
        }


        private void rbtnForceOutput_Click(object sender, EventArgs e)
        {
            var radioButton = sender as RadioButton;
            int pin_id;

            if (radioButton == null)
                return;

            if (int.TryParse(radioButton.Tag.ToString(), out pin_id) == false)
                return;

            if (radioButton.Checked)
            {
                if (clientInterface.SetResetDIO(pin_id, true))
                    radioButton.Checked = false;
            }
            else
            {
                if (clientInterface.SetResetDIO(pin_id, false))
                    radioButton.Checked = true;
            }

            return;
        }


        private void btnNewRegister_Click(object sender, EventArgs e)
        {
            txtBoxName.Text = string.Empty;
            txtBoxShift.Text = string.Empty;
        }


        private void btnSaveRegister_Click(object sender, EventArgs e)
        {
            Cadastro newCadastro = new Cadastro();
            
            if (txtBoxName.Text != string.Empty)
                newCadastro.Operador = txtBoxName.Text;

            if (txtBoxShift.Text != string.Empty)
                newCadastro.Turno = txtBoxShift.Text;

            foreach (Cadastro cadastro in Cadastros)
            {
                if (cadastro.Operador == newCadastro.Operador)
                {
                    MessageBox.Show("Cadastro existente", "Novo Cadastro");
                    return;
                }
            }

            Cadastros.Add(newCadastro);

            RegisterStorage.UpdateXML(Cadastros);
        }


        private void btnExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }


        private void btnGenerateSpreadsheet_Click(object sender, EventArgs e)
        {
            string[] files, fields;
            string path;
            var csvFile = new StringBuilder();
            int ix, iy;

            if (string.IsNullOrWhiteSpace(manualLocation.IPAddress))
            {
                MessageBox.Show("Endereco IP invalido.", "Geracao de Planilha");
                return;
            }

            if (string.IsNullOrWhiteSpace(txtBoxNomePlanilha.Text))
            {
                MessageBox.Show("Insira um nome antes de gerar planilha.", "Geracao de Planilha");
                return;
            }
            else
            {
                if (File.Exists(txtBoxNomePlanilha.Text + ".xlsx"))
                {
                    DialogResult = MessageBox.Show("Uma planilha com o mesmo nome existe. Deseja gerar uma planilha com novo nome?", "Geracao de Planilha", MessageBoxButtons.OKCancel);
                    if (DialogResult == DialogResult.Cancel)
                    {
                        return;
                    }
                    else
                    {
                        txtBoxNomePlanilha.Text = txtBoxNomePlanilha.Text + "_" + DateTime.Now.ToString("mm_dd_yyyy_hh_mm_ss");
                    }
                }
            }

            // use verbatim string literal @
            path = @"\\" + manualLocation.IPAddress + @"\" + "share";
            
            if (Directory.Exists(path) == false)
            {
                MessageBox.Show("Caminho:" + path + " invalido. Verifique se direotrio esta disponivel.", "Geracao de Planilha");
                return;
            }

            files = Directory.GetFiles(path);

            using (var package = new ExcelPackage())
            {
                ExcelWorksheet worksheet = package.Workbook.Worksheets.Add("Indicador");

                iy = 1; // set index to the first row

                foreach (string file in files)
                {
                    /* skip if not a csv file */
                    if (file.Contains(".csv") == false)
                        continue;

                    var parser = new TextFieldParser(file);
                    parser.TextFieldType = FieldType.Delimited;
                    parser.SetDelimiters(",");

                    while (parser.EndOfData == false)
                    {
                        fields = parser.ReadFields();
                        ix = 1; // set index to the start of the row (A)

                        foreach (string field in fields)
                        {
                            // 65 - ASCII offset
                            Char cx = (Char)((65) + (ix - 1));

                            // index for cells is A1, A3, B45, etc...
                            worksheet.Cells[cx.ToString() + iy.ToString()].Value = field;

                            ix++; // increment columm
                        }
                        iy++; // increment row
                    }
                }

                package.Workbook.Properties.Title = "Indicadores";
                package.Workbook.Properties.Author = "Insul";
                package.Workbook.Properties.Company = "Insul";
                package.Workbook.Properties.Comments = "Indicador de maquina parada";

                var xlFile = new FileInfo(txtBoxNomePlanilha.Text + ".xlsx");
                // save our new workbook in the output directory and we are done!
                package.SaveAs(xlFile);
            }
        }


        private void btnAbrirWeb_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("http://" + clientInterface.IP);
        }


        private void cbOperator_Click(object sender, EventArgs e)
        {
            cbOperator.Items.Clear();

            foreach (Cadastro cadastro in Cadastros)
            {
                cbOperator.Items.Add(cadastro.Operador);
            }
        }

        private void comboBox1_Click(object sender, EventArgs e)
        {
            cbEditRegister.Items.Clear();

            foreach (Cadastro cadastro in Cadastros)
            {
                cbEditRegister.Items.Add(cadastro.Operador);
            }
        }

        private void btnDeleteRegister_Click(object sender, EventArgs e)
        {
            foreach (Cadastro cadastro in Cadastros)
            {
                if (cadastro.Operador == cbEditRegister.Text)
                {
                    Cadastros.Remove(cadastro);
                    RegisterStorage.UpdateXML(Cadastros);
                    break;
                }
            }            
        }

        private void btnAdmin_Click(object sender, EventArgs e)
        {
            string result = string.Empty;
            var btn = sender as Button;

            if (admin_mode)
            {
                tabControl.TabPages.Remove(tabPageRelatorio);
                tabControl.TabPages.Remove(tabPageRegister);
                tabControl.TabPages.Remove(tabPageConfig);

                btn.BackColor = SystemColors.Control;
            }
            else
            {
                DialogResult = InputBox.Show("Modo de Administrador", "Insira a senha", ref result);

                if (DialogResult == DialogResult.OK)
                {
                    if (result == ADMIN_PASSWORD)
                    {
                        // Modo admin
                        tabControl.TabPages.AddRange(new TabPage[] { tabPageConfig, tabPageRegister, tabPageRelatorio });
                        admin_mode = true;

                        btn.BackColor = Color.Green;
                    }
                    else
                    {
                        MessageBox.Show("Modo de Administrador", "Senha Incorreta", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }
    }
}
