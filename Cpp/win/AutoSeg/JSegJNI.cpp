#include <iostream>
//#include "JSegJNI.h"
#include <jni.h>
#include "AutoTokenizer.hpp"
#include "CodeUtils.hpp"

using namespace std;
using namespace mingspy;


static ITokenizer & GetTokenizer()
{
    static AutoTokenizer t;
    return t;
}


static jobject toJavaTokenList(JNIEnv * env, const vector<Token> & result, bool addNature = false)
{
    jclass list_cls = env->FindClass("Ljava/util/ArrayList;");//获得ArrayList类引用
    jmethodID list_costruct = env->GetMethodID(list_cls , "<init>","()V"); //获得得构造函数Id
    jmethodID list_add  = env->GetMethodID(list_cls,"add","(Ljava/lang/Object;)Z");
    jobject list_obj = env->NewObject(list_cls , list_costruct);
    jclass token_cls = env->FindClass("Lcom/mingspy/jseg/Token;");
    jmethodID token_costruct = env->GetMethodID(token_cls , "<init>", "(II)V");
    jfieldID token_nature_id = env->GetFieldID(token_cls,"nature","Ljava/lang/String;");
    for(int i = 0 ; i < result.size(); i++) {
        jobject t_obj = env->NewObject(token_cls , token_costruct , result[i]._off,result[i]._len);
        if(addNature) {
            wstring nature = DictFactory::LexicalDict().getNature(result[i]._attr);
            env->SetObjectField(t_obj, token_nature_id,
                                env->NewString((const jchar *)nature.c_str(), nature.size()));
        }
        env->CallBooleanMethod(list_obj , list_add , t_obj);
    }
    return list_obj ;
}


/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    MaxSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
jobject Java_MaxSplit
(JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    vector<Token> result;
    GetTokenizer().maxSplit(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);
    return toJavaTokenList(env,result);
}

/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    FullSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
jobject Java_FullSplit
(JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    vector<Token> result;
    GetTokenizer().fullSplit(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);
    return toJavaTokenList(env,result);
}


/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    UniGramSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
jobject Java_UniGramSplit
(JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    vector<Token> result;
    GetTokenizer().uniGramSplit(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);
    return toJavaTokenList(env,result);
}

/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    BiGramSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
jobject Java_BiGramSplit
(JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    vector<Token> result;
    GetTokenizer().biGramSplit(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);
    return toJavaTokenList(env,result);
}

/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    MixSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
jobject Java_MixSplit
(JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    vector<Token> result;
    GetTokenizer().mixSplit(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);
    return toJavaTokenList(env,result);
}

/*
 * Class:     com_mingspy_jseg_JSegJNI
 * Method:    MixSplit
 * Signature: (Ljava/lang/String;)Ljava/util/List;
 */
jobject Java_POSTagging
(JNIEnv * env, jobject obj, jstring jstr)
{
    const char * p = env->GetStringUTFChars(jstr, 0);
    vector<Token> result;
    cout<<"input=>"<<p<<endl;
    GetTokenizer().posTagging(Utf8ToUnicode(p), result);
    env->ReleaseStringUTFChars(jstr, p);

    return toJavaTokenList(env,result, true);
}


static JNINativeMethod s_methods[] = {
    {"MaxSplit", "(Ljava/lang/String;)Ljava/util/List;", (void*)Java_MaxSplit},
    {"FullSplit", "(Ljava/lang/String;)Ljava/util/List;", (void*)Java_FullSplit},
    {"UniGramSplit", "(Ljava/lang/String;)Ljava/util/List;", (void*)Java_UniGramSplit},
    {"BiGramSplit", "(Ljava/lang/String;)Ljava/util/List;", (void*)Java_BiGramSplit},
    {"MixSplit", "(Ljava/lang/String;)Ljava/util/List;", (void*)Java_MixSplit},
    {"POSTagging", "(Ljava/lang/String;)Ljava/util/List;", (void*)Java_POSTagging},
};

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv* env = NULL;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return JNI_ERR;
    }

    jclass cls = env->FindClass("Lcom/mingspy/jseg/JSegJNI;");
    if (cls == NULL) {
        return JNI_ERR;
    }

    int len = sizeof(s_methods) / sizeof(s_methods[0]);
    if (env->RegisterNatives(cls, s_methods, len) < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_4;
}


