using FileHelpers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MQTT_Client.tools
{
    class Spreadsheet
    {
        string path;
        string fileName = ""; // TODO: testing var
        string[] headers = new string[] {
            "Data Hora Parada",
            "Motivo Parada",
            "Maquina",
            "Operador",
            "Turno",
            "Produto" };

        public Spreadsheet(string path)
        {
            this.path = path;
        }

        public string Path
        {
            set { path = value; }
            get { return path; }
        }

        public void GenerateSpreasheet()
        {
            var engine = new FileHelperEngine<Register>();
            var registers = engine.ReadFile(fileName);

            foreach (var reg in registers)
            {
            }
        }

        class Register
        {
            public string StopDateTime;
            public string StopReason;
            public string MachineName;
            public string User;
            public int ShiftID;
            public int ProductID;
        }
    }
}
