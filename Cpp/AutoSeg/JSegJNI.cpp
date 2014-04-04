#include <iostream>
#include "JSegJNI.h"
#include "AutoTokenizer.hpp"
#include "CodeUtils.hpp"

using namespace std;
using namespace mingspy;

static ITokenizer & GetTokenizer(){
    static AutoTokenizer t;
    return t;
}


static jobject toJavaList(JNIEnv * env, const vector<Token> & result){
    jclass list_cls = env->FindClass("Ljava/util/ArrayList;");//获得ArrayList类引用
    jmethodID list_costruct = env->GetMethodID(list_cls , "<init>","()V"); //获得得构造函数Id
    jmethodID list_add  = env->GetMethodID(list_cls,"add","(Ljava/lang/Object;)Z");
    jobject list_obj = env->NewObject(list_cls , list_costruct);  
    jclass token_cls = env->FindClass("Lcom/mingspy/jseg/Token;");  
    jmethodID token_costruct = env->GetMethodID(token_cls , "<init>", "(II)V"); 
    for(int i = 0 ; i < result.size(); i++)  
    {  
        jobject t_obj = env->NewObject(token_cls , token_costruct , result[i]._off,result[i]._len); 
        env->CallBooleanMethod(list_obj , list_add , t_obj);
    }
    return list_obj ;
}
/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    SetDictFolder
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_mingspy_jseg_JSegJNI_SetDictFolder
  (JNIEnv * env, jobject obj, jstring jstr)
{
    const char * pchars = env->GetStringUTFChars(jstr, 0);
    DictFactory::setDictDir(pchars);
    env->ReleaseStringUTFChars(jstr, pchars);
    return true;
}

/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    MaxSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_com_mingspy_jseg_JSegJNI_MaxSplit
  (JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    vector<Token> result;
    GetTokenizer().maxSplit(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);
    return toJavaList(env,result);
}

/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    FullSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_com_mingspy_jseg_JSegJNI_FullSplit
  (JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    vector<Token> result;
    GetTokenizer().fullSplit(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);
    return toJavaList(env,result);
}

/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    OneGramSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_com_mingspy_jseg_JSegJNI_OneGramSplit
  (JNIEnv * env, jobject obj, jstring jstr){
      const char * p = env->GetStringUTFChars(jstr, 0);
      vector<Token> result;
      GetTokenizer().oneGramSplit(Utf8ToUnicode(p), result);
      env->ReleaseStringUTFChars(jstr, p);
      return toJavaList(env,result);
}

/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    Test
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_mingspy_jseg_JSegJNI_Test
  (JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    cout<<"input is:"<<p<<endl;

    vector<Token> result;
    GetTokenizer().maxSplit(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);
    for(int i = 0; i< result.size(); i++){
        cout<<"("<<result[i]._off<<","<<result[i]._len<<")";
    }
}

