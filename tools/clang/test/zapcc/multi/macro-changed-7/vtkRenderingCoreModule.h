#define VTK_AUTOINIT(M) VTK_AUTOINIT0(, M##_AUTOINIT)
#define VTK_AUTOINIT0(M, T) VTK_AUTOINIT1(, T)
#define VTK_AUTOINIT1(M, T)                                                    \
  VTK_AUTOINIT_DECLARE_##T struct _AutoInit {                                  \
    _AutoInit() { VTK_AUTOINIT_DESTRUCT_##T }                                  \
  } M_AutoInit_Instance;
#define VTK_AUTOINIT_DECLARE_2(t1, t2) VTK_AUTOINIT_DECLARE(t2)
#define VTK_AUTOINIT_DECLARE_3(t1, t2, t3) VTK_AUTOINIT_DECLARE_2(, t2)
#define VTK_AUTOINIT_DECLARE(M) void M##_AutoInit_Destruct();
#define VTK_AUTOINIT_DESTRUCT_2(t1, t2) VTK_AUTOINIT_DESTRUCT(t2)
#define VTK_AUTOINIT_DESTRUCT_3(t1, t2, t3) VTK_AUTOINIT_DESTRUCT_2(, t2)
#define VTK_AUTOINIT_DESTRUCT(M) M##_AutoInit_Destruct();
VTK_AUTOINIT(vtkRenderingCore)
