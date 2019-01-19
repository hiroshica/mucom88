using System.Runtime.InteropServices; // DLL Import

namespace testmain
{
    class Mucom88Dll
    {
        [DllImport("mucom88dll.Dll", EntryPoint = "CreateVM")]
        public static extern int CreateVM();   // 返り値 -1 = error : 0 > インデックス番号
        [DllImport("mucom88dll.Dll", EntryPoint = "RemoveVM")]
        public static extern bool RemoveVM(int index = -1);
        [DllImport("mucom88dll.Dll", EntryPoint = "Init")]
        public static extern void Init(int index = -1, int option = 0);
        [DllImport("mucom88dll.Dll", EntryPoint = "Reset")]
        public static extern void Reset(int index = -1, int option = 0);
    }
}
