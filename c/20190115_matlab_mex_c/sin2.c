/*
 * sfuntmpl_basic.c: Basic 'C' template for a level 2 S-function.
 *
 * Copyright 1990-2013 The MathWorks, Inc.
 */


/*
 * You must specify the S_FUNCTION_NAME as the name of your S-function
 * (i.e. replace sfuntmpl_basic with the name of your S-function).
 */

#define S_FUNCTION_NAME  sin2   //这里的sin2一定要与c文件的文件名一致
#define S_FUNCTION_LEVEL 2

/*Level 2 M文件S函数----支持访问更多地S函数API，支持代码生成。当你要使用M文件
 * 来实现一个S函数的时候，选择Level 2 M文件S函数。
 */

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"
//程序里用到的头文件在这里引用

/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
 //这个函数用来设置输入、输出和参数
    ssSetNumSFcnParams(S, 0);  /* Number of expected parameters */
    //设置参数个数，这里为0
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        /* Return if number of expected != number of actual parameters */
        return;
    }

   // ssSetNumContStates(S, 0);   //设置连续状态的个数，缺省为0
  //  ssSetNumDiscStates(S, 0);   //设置离散状态的个数，缺省为0

    if (!ssSetNumInputPorts(S, 1)) return;  //设置输入变量的个数，这里为1
    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);   //设置输入变量0的维数为动态的
   // ssSetInputPortRequiredContiguous(S, 0, 1); /*direct input signal access*/
    /*
     * Set direct feedthrough flag (1=yes, 0=no).
     * A port has direct feedthrough if the input is used in either
     * the mdlOutputs or mdlGetTimeOfNextVarHit functions.
     */
    /*
     *设置input0的访问方式，true就是临近访问，这样指针的增量后
     * 就可以直接访问下个input端口
     */
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    //设置输入端口的信号是否在mdlOutputs函数中使用，这里设置为true
    if (!ssSetNumOutputPorts(S, 1)) return; 
    //设置输出变量的个数，这里为1
    ssSetOutputPortWidth(S, 0, DYNAMICALLY_SIZED);    //设置输出变量0的维数为动态
   // ssSetOutputPortWidth(S, 1, 1);    //设置输出变量1的维数为1
    
    ssSetNumSampleTimes(S, 1);  //设置采样时间1s
    //ssSetNumRWork(S, 0);
   // ssSetNumIWork(S, 0);
    //ssSetNumPWork(S, 0);
    //ssSetNumModes(S, 0);
   // ssSetNumNonsampledZCs(S, 0);

    /* Specify the sim state compliance to be same as a built-in block */
   // ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);

    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
 
}

/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);

}

//#define MDL_INITIALIZE_CONDITIONS   /* Change to #undef to remove function */
//#if defined(MDL_INITIALIZE_CONDITIONS)
  /* Function: mdlInitializeConditions ========================================
   * Abstract:
   *    In this function, you should initialize the continuous and discrete
   *    states for your S-function block.  The initial states are placed
   *    in the state vector, ssGetContStates(S) or ssGetRealDiscStates(S).
   *    You can also perform any other initialization activities that your
   *    S-function may require. Note, this routine will be called at the
   *    start of simulation and if it is present in an enabled subsystem
   *    configured to reset states, it will be call when the enabled subsystem
   *    restarts execution to reset the states.
   */
 
static void mdlOutputs(SimStruct *S, int_T tid)
{
    int_T i;
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs ( S,0 );
    real_T *y = ssGetOutputPortRealSignal (S,0 );
    int_T width = ssGetOutputPortWidth ( S,0 );
    for ( i=0;i<width;i++ ) {
        *y++ = 1.0 * add(1, 2) * ( *uPtrs[ i ] ); // calllib('dll', 'add', 1, 2)
    }
}

static void mdlTerminate(SimStruct *S)
{
}

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
