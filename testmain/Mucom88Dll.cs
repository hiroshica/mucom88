using System.Runtime.InteropServices; // DLL Import

namespace testmain
{
    class Mucom88Dll
    {
        [DllImport("mucom88dll.Dll", EntryPoint = "Mucom88Core::CreateVM")]
        public static extern int CreateVM();
        [DllImport("mucom88dll.Dll", EntryPoint = "Mucom88Core::RemoveVM")]
        public static extern bool RemoveVM(int index = -1);
        [DllImport("mucom88dll.Dll", EntryPoint = "Mucom88Core::Init")]
        public static extern void Init(int index = -1, int option = 0);
        [DllImport("mucom88dll.Dll", EntryPoint = "Mucom88Core::Reset")]
        public static extern void Reset(int index = -1, int option = 0);
    }
}
