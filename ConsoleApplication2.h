// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// CONSOLEAPPLICATION1_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.

#include <iostream>

#ifdef CONSOLEAPPLICATION2_EXPORTS
#define CONSOLEAPPLICATION2_API __declspec(dllexport)
#else
#define CONSOLEAPPLICATION2_API __declspec(dllimport)
#endif

#ifdef __cplusplus
#define EXTERN_C	extern "C"
#define EXTERN_C_START extern "C"{
#define EXTERN_C_END }
#else
#define EXTERN_C	extern
#define EXTERN_CNSTART
#define EXTERN_C_END
#endif

EXTERN_C_START
// input�� ���ٰ� ������ �ƴ�.
int CONSOLEAPPLICATION2_API _stdcall Func(char *infilename,int data_size, int num_bits_per_sample);

EXTERN_C_END
