using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace testmain
{
    public partial class Form1 : Form
    {
        int getindex;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            getindex = Mucom88Dll.Mucom88CoreCreateVM();
            Mucom88Dll.Mucom88CoreInit(getindex);
            Mucom88Dll.Mucom88CoreReset(getindex);
            Mucom88Dll.Mucom88CoreRemoveVM();
        }
    }
}
