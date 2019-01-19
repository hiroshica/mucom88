using System.Runtime.InteropServices; // DLL Import

namespace testmain
{
    class Mucom88Dll
    {
        [DllImport("mucom88dll.Dll", EntryPoint = "Mucom88CoreCreateVM", CallingConvention = CallingConvention.StdCall)]
        public static extern int Mucom88CoreCreateVM();
        [DllImport("mucom88dll.Dll", EntryPoint = "Mucom88CoreRemoveVM", CallingConvention = CallingConvention.StdCall)]
        public static extern bool Mucom88CoreRemoveVM(int index = -1);
        [DllImport("mucom88dll.Dll", EntryPoint = "Mucom88CoreInit", CallingConvention = CallingConvention.StdCall)]
        public static extern void Mucom88CoreInit(int index = -1, int option = 0);
        [DllImport("mucom88dll.Dll", EntryPoint = "Mucom88CoreReset", CallingConvention = CallingConvention.StdCall)]
        public static extern void Mucom88CoreReset(int index = -1, int option = 0);
    }
}
