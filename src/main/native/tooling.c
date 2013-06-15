#include <tooling.h>
#include <jvmti.h>
#include <string.h>
#include <stdarg.h>

#define MOD_STATIC 0x00000008

typedef struct {
  jvmtiEnv *jvmti;
  jobject tools;
  jmethodID createFrame/*Thread, Method, I height, Object this, J position,
			 LocalVariable[], Object[] locals*/;
  jmethodID createLocal/*J start,I len, String signature, I slot, String name*/;
  jmethodID popFrame/*Thread,I height,J position,Object[] locals*/;
  jclass LocalVariable;
  jclass CallFrame;
  jclass Object;
  // boxing
  jmethodID boxLong;
  jmethodID boxFloat;
  jmethodID boxDouble;
  jmethodID boxInt;
  jmethodID boxShort;
  jmethodID boxChar;
  jmethodID boxByte;
  jmethodID boxBool;
  // unboxing
  jmethodID unboxLong;
  jmethodID unboxFloat;
  jmethodID unboxDouble;
  jmethodID unboxInt;
  jmethodID unboxShort;
  jmethodID unboxChar;
  jmethodID unboxByte;
  jmethodID unboxBool;
} AgentData;

static AgentData *agent = NULL;

jboolean InitializeJNI(JNIEnv *env, jclass tools) {
  jmethodID m;
  jclass c;

  if (!agent->createFrame) {
    m = (*env)->GetMethodID(env, tools, "createFrame",
			    "(Ljava/lang/Thread;Ljava/lang/reflect/Method;ILjava/lang/Object;J[Lorg/thobe/java/tooling/LocalVariable;[Ljava/lang/Object;)Lorg/thobe/java/tooling/CallFrame;");
    if (!m) return JNI_FALSE;
    agent->createFrame = m;
  }

  if (!agent->createLocal) {
    m = (*env)->GetMethodID(env, tools, "createLocal",
			    "(JILjava/lang/String;ILjava/lang/String;)Lorg/thobe/java/tooling/LocalVariable;");
    if (!m) return JNI_FALSE;
    agent->createLocal = m;
  }

  if (!agent->popFrame) {
    m = (*env)->GetMethodID(env, tools, "popFrame", "(Ljava/lang/Thread;IJ[Ljava/lang/Object;)V");
    if (!m) return JNI_FALSE;
    agent->popFrame = m;
  }

  if (!agent->LocalVariable) {
    c = (*env)->FindClass(env, "org/thobe/java/tooling/LocalVariable");
    if (!c) return JNI_FALSE;
    agent->LocalVariable = (jclass)((*env)->NewGlobalRef(env, c));
    (*env)->DeleteLocalRef(env, c);
  }

  if (!agent->CallFrame) {
    c = (*env)->FindClass(env, "org/thobe/java/tooling/CallFrame");
    if (!c) return JNI_FALSE;
    agent->CallFrame = (jclass)((*env)->NewGlobalRef(env, c));
    (*env)->DeleteLocalRef(env, c);
  }

  if (!agent->Object) {
    c = (*env)->FindClass(env, "java/lang/Object");
    if (!c) return JNI_FALSE;
    agent->Object = (jclass)((*env)->NewGlobalRef(env, c));
    (*env)->DeleteLocalRef(env, c);
  }

  // boxing methods
  if (!agent->boxLong) {
    m = (*env)->GetMethodID(env, tools, "boxLong", "(J)Ljava/lang/Object;");
    if (!m) return JNI_FALSE;
    agent->boxLong = m;
  }
  if (!agent->boxFloat) {
    m = (*env)->GetMethodID(env, tools, "boxFloat", "(F)Ljava/lang/Object;");
    if (!m) return JNI_FALSE;
    agent->boxFloat = m;
  }
  if (!agent->boxDouble) {
    m = (*env)->GetMethodID(env, tools, "boxDouble", "(D)Ljava/lang/Object;");
    if (!m) return JNI_FALSE;
    agent->boxDouble = m;
  }
  if (!agent->boxInt) {
    m = (*env)->GetMethodID(env, tools, "boxInt", "(I)Ljava/lang/Object;");
    if (!m) return JNI_FALSE;
    agent->boxInt = m;
  }
  if (!agent->boxShort) {
    m = (*env)->GetMethodID(env, tools, "boxShort", "(S)Ljava/lang/Object;");
    if (!m) return JNI_FALSE;
    agent->boxShort = m;
  }
  if (!agent->boxChar) {
    m = (*env)->GetMethodID(env, tools, "boxChar", "(C)Ljava/lang/Object;");
    if (!m) return JNI_FALSE;
    agent->boxChar = m;
  }
  if (!agent->boxByte) {
    m = (*env)->GetMethodID(env, tools, "boxByte", "(B)Ljava/lang/Object;");
    if (!m) return JNI_FALSE;
    agent->boxByte = m;
  }
  if (!agent->boxBool) {
    m = (*env)->GetMethodID(env, tools, "boxBool", "(Z)Ljava/lang/Object;");
    if (!m) return JNI_FALSE;
    agent->boxBool = m;
  }

  // unboxing methods
  if (!agent->unboxLong) {
    m = (*env)->GetMethodID(env, tools, "unboxLong", "(Ljava/lang/Object;)J");
    if (!m) return JNI_FALSE;
    agent->unboxLong = m;
  }
  if (!agent->unboxFloat) {
    m = (*env)->GetMethodID(env, tools, "unboxFloat", "(Ljava/lang/Object;)F");
    if (!m) return JNI_FALSE;
    agent->unboxFloat = m;
  }
  if (!agent->unboxDouble) {
    m = (*env)->GetMethodID(env, tools, "unboxDouble", "(Ljava/lang/Object;)D");
    if (!m) return JNI_FALSE;
    agent->unboxDouble = m;
  }
  if (!agent->unboxInt) {
    m = (*env)->GetMethodID(env, tools, "unboxInt", "(Ljava/lang/Object;)I");
    if (!m) return JNI_FALSE;
    agent->unboxInt = m;
  }
  if (!agent->unboxShort) {
    m = (*env)->GetMethodID(env, tools, "unboxShort", "(Ljava/lang/Object;)S");
    if (!m) return JNI_FALSE;
    agent->unboxShort = m;
  }
  if (!agent->unboxChar) {
    m = (*env)->GetMethodID(env, tools, "unboxChar","(Ljava/lang/Object;)C");
    if (!m) return JNI_FALSE;
    agent->unboxChar = m;
  }
  if (!agent->unboxByte) {
    m = (*env)->GetMethodID(env, tools, "unboxByte", "(Ljava/lang/Object;)B");
    if (!m) return JNI_FALSE;
    agent->unboxByte = m;
  }
  if (!agent->unboxBool) {
    m = (*env)->GetMethodID(env, tools, "unboxBool", "(Ljava/lang/Object;)Z");
    if (!m) return JNI_FALSE;
    agent->unboxBool = m;
  }

  return JNI_TRUE;
}

jint throwException(JNIEnv *env, const char *name, const char *format, ...) {
  jclass clazz;
  char message[128];
  va_list args;

  va_start(args, format);
  {
    vsprintf(message, format, args);
  }
  va_end(args);

  clazz = (*env)->FindClass(env, name);
  return (*env)->ThrowNew(env, clazz, message);
}

jboolean verifyTool(JNIEnv *env, jobject tools) {
  if ((*env)->IsSameObject(env, agent->tools, tools)) {
    return JNI_TRUE;
  } else {
    throwException(env, "java/lang/SecurityException",
		   "Invalid access to ToolingInterface.");
    return JNI_FALSE;
  }
}

#define decodes(CASE) case CASE: return #CASE
const char* decodeErrorCode(jvmtiError tiErr) {
  switch(tiErr) {
    decodes(JVMTI_ERROR_NONE);
    decodes(JVMTI_ERROR_INVALID_THREAD);
    decodes(JVMTI_ERROR_INVALID_THREAD_GROUP);
    decodes(JVMTI_ERROR_INVALID_PRIORITY);
    decodes(JVMTI_ERROR_THREAD_NOT_SUSPENDED);
    decodes(JVMTI_ERROR_THREAD_SUSPENDED);
    decodes(JVMTI_ERROR_THREAD_NOT_ALIVE);
    decodes(JVMTI_ERROR_INVALID_OBJECT);
    decodes(JVMTI_ERROR_INVALID_CLASS);
    decodes(JVMTI_ERROR_CLASS_NOT_PREPARED);
    decodes(JVMTI_ERROR_INVALID_METHODID);
    decodes(JVMTI_ERROR_INVALID_LOCATION);
    decodes(JVMTI_ERROR_INVALID_FIELDID);
    decodes(JVMTI_ERROR_NO_MORE_FRAMES);
    decodes(JVMTI_ERROR_OPAQUE_FRAME);
    decodes(JVMTI_ERROR_TYPE_MISMATCH);
    decodes(JVMTI_ERROR_INVALID_SLOT);
    decodes(JVMTI_ERROR_DUPLICATE);
    decodes(JVMTI_ERROR_NOT_FOUND);
    decodes(JVMTI_ERROR_INVALID_MONITOR);
    decodes(JVMTI_ERROR_NOT_MONITOR_OWNER);
    decodes(JVMTI_ERROR_INTERRUPT);
    decodes(JVMTI_ERROR_INVALID_CLASS_FORMAT);
    decodes(JVMTI_ERROR_CIRCULAR_CLASS_DEFINITION);
    decodes(JVMTI_ERROR_FAILS_VERIFICATION);
    decodes(JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_ADDED);
    decodes(JVMTI_ERROR_UNSUPPORTED_REDEFINITION_SCHEMA_CHANGED);
    decodes(JVMTI_ERROR_INVALID_TYPESTATE);
    decodes(JVMTI_ERROR_UNSUPPORTED_REDEFINITION_HIERARCHY_CHANGED);
    decodes(JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_DELETED);
    decodes(JVMTI_ERROR_UNSUPPORTED_VERSION);
    decodes(JVMTI_ERROR_NAMES_DONT_MATCH);
    decodes(JVMTI_ERROR_UNSUPPORTED_REDEFINITION_CLASS_MODIFIERS_CHANGED);
    decodes(JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_MODIFIERS_CHANGED);
    decodes(JVMTI_ERROR_UNMODIFIABLE_CLASS);
    decodes(JVMTI_ERROR_NOT_AVAILABLE);
    decodes(JVMTI_ERROR_MUST_POSSESS_CAPABILITY);
    decodes(JVMTI_ERROR_NULL_POINTER);
    decodes(JVMTI_ERROR_ABSENT_INFORMATION);
    decodes(JVMTI_ERROR_INVALID_EVENT_TYPE);
    decodes(JVMTI_ERROR_ILLEGAL_ARGUMENT);
    decodes(JVMTI_ERROR_NATIVE_METHOD);
    decodes(JVMTI_ERROR_CLASS_LOADER_UNSUPPORTED);
    decodes(JVMTI_ERROR_OUT_OF_MEMORY);
    decodes(JVMTI_ERROR_ACCESS_DENIED);
    decodes(JVMTI_ERROR_WRONG_PHASE);
    decodes(JVMTI_ERROR_INTERNAL);
    decodes(JVMTI_ERROR_UNATTACHED_THREAD);
    decodes(JVMTI_ERROR_INVALID_ENVIRONMENT);
  default:
    return NULL;
  }
}

void logError(const char *where, const char *what, jvmtiError tiErr)
{
  const char *err;
  err = decodeErrorCode(tiErr);
  if (err)
    fprintf(stderr, "AGENT ERROR: %s, JVMTI error in %s: %s\n",
                                  where,             what,err);
  else
    fprintf(stderr, "AGENT ERROR: %s, JVMTI error in %s, code: 0x%x\n",
                                  where,             what,     tiErr);
}

jint throwJvmtiException(JNIEnv *env, const char *tiMethod, jvmtiError tiErr) {
  const char *err;
  err = decodeErrorCode(tiErr);
  if (err) {
    return throwException(env, "java/lang/Error",
			  "Unexpected JVMTI failure in %s: %s.",
			  tiMethod, err);
  } else {
    return throwException(env, "java/lang/Error",
			  "Unexpected JVMTI failure in %s, error code: 0x%x.",
			  tiMethod, tiErr);
  }
}

jobject createLocal(JNIEnv *env, jobject tools, jvmtiLocalVariableEntry entry) {
  jobject name;
  jobject signature;
  jobject local;

  name = (*env)->NewStringUTF(env, entry.name);
  signature = (*env)->NewStringUTF(env, entry.signature);

  local = (*env)->CallObjectMethod(env, tools, agent->createLocal,
				   entry.start_location, entry.length,
				   signature, entry.slot, name);

  (*env)->DeleteLocalRef(env, signature);
  (*env)->DeleteLocalRef(env, name);

  if ((*env)->ExceptionCheck(env)) return NULL;

  return local;
}

jobject getLocal(JNIEnv *env,jthread thread,jchar type,jint depth,jint slot) {
  jvmtiError tiErr;
  jobject result;
  jint iValue;
  jlong jValue;
  jfloat fValue;
  jdouble dValue;

  // get the variable
  switch(type) {
  case '[': // array (is an Object)
  case 'L': // Object
    tiErr = (*(agent->jvmti))->GetLocalObject(agent->jvmti, thread,
					      depth, slot, &result);
    break;
  case 'J': // long
    tiErr = (*(agent->jvmti))->GetLocalLong(agent->jvmti, thread,
					    depth, slot, &jValue);
    break;
  case 'F': // float
    tiErr = (*(agent->jvmti))->GetLocalFloat(agent->jvmti, thread,
					     depth, slot, &fValue);
    break;
  case 'D': // double
    tiErr = (*(agent->jvmti))->GetLocalDouble(agent->jvmti, thread,
					      depth, slot, &dValue);
    break;
  case 'I': // int
  case 'S': // short
  case 'C': // char
  case 'B': // byte
  case 'Z': // boolean
    tiErr = (*(agent->jvmti))->GetLocalInt(agent->jvmti, thread,
					   depth, slot, &iValue);
    break;
  default:
    throwException(env, "java/lang/IllegalArgumentException",
		   "Invalid type identifier [%c].", type);
    return NULL;
  }

  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetLocal<value>", tiErr);
    return NULL;
  }

  // Box primitives
  switch(type) {
  case 'J': // long
    result = (*env)->CallObjectMethod(env,agent->tools,agent->boxLong,jValue);
    break;
  case 'F': // float
    result = (*env)->CallObjectMethod(env,agent->tools,agent->boxFloat,fValue);
    break;
  case 'D': // double
    result = (*env)->CallObjectMethod(env,agent->tools,agent->boxDouble,dValue);
    break;
    // INTEGER TYPES
  case 'I': // int
    result = (*env)->CallObjectMethod(env,agent->tools,agent->boxInt,iValue);
    break;
  case 'S': // short
    result = (*env)->CallObjectMethod(env,agent->tools,agent->boxShort,iValue);
    break;
  case 'C': // char
    result = (*env)->CallObjectMethod(env,agent->tools,agent->boxChar,iValue);
    break;
  case 'B': // byte
    result = (*env)->CallObjectMethod(env,agent->tools,agent->boxByte,iValue);
    break;
  case 'Z': // boolean
    result = (*env)->CallObjectMethod(env,agent->tools,agent->boxBool,iValue);
    break;
  }

  return result;
}

jobjectArray getAllLocals(JNIEnv *env, jthread thread, jint depth,
			  jlocation location, jint count,
			  jvmtiLocalVariableEntry *locals) {
  jint i;
  jint highSlot;
  jobject local;
  jobjectArray result;
  
  highSlot = 0;

  for (i = 0; i < count; i++) {
    if (locals[i].start_location <= location &&
	locals[i].start_location + locals[i].length >= location) {
      if (locals[i].slot > highSlot) highSlot = locals[i].slot;
    }
  }

  result = (*env)->NewObjectArray(env, highSlot+1, agent->Object, NULL);
  if ((*env)->ExceptionCheck(env)) {
    return NULL;
  }

  for (i = 0; i < count; i++) {
    if (locals[i].start_location <= location &&
	locals[i].start_location + locals[i].length >= location) {
      local = getLocal(env, thread, locals[i].signature[0], 0, locals[i].slot);
      if ((*env)->ExceptionCheck(env)) {
	return NULL;
      }
      (*env)->SetObjectArrayElement(env, result, locals[i].slot, local);
      (*env)->DeleteLocalRef(env, local);
      if ((*env)->ExceptionCheck(env)) {
	return NULL;
      }
    }
  }

  return result;
}

jobject createFrame(JNIEnv *env, jobject tools, jthread thread, jint totalDepth,
		    jint depth, jboolean live, jmethodID method,
		    jlocation location,jboolean get_locals,jboolean isNative) {
  jvmtiError tiErr;
  jvmtiLocalVariableEntry *locals;
  jint count;
  jint i;
  jboolean isStatic;
  jobjectArray variables;
  jobjectArray lvars;
  jobject var;
  jobject this;
  jobject frame;
  jclass declaringClass;
  jobject reflectedMethod;

  lvars = NULL; // initialize

  if (get_locals && !isNative) {
    tiErr = (*(agent->jvmti))->GetLocalVariableTable(agent->jvmti, method,
						     &count, &locals);
    if (tiErr != JVMTI_ERROR_NONE) {
      throwJvmtiException(env, "GetLocalVariableTable", tiErr);
      return NULL;
    }
    variables = (*env)->NewObjectArray(env, count, agent->LocalVariable, NULL);
    if (!variables || (*env)->ExceptionCheck(env)) return NULL;
    for (i = 0; i < count; i++) {
      var = createLocal(env, tools, locals[i]);
      if (!var) {
	(*(agent->jvmti))->Deallocate(agent->jvmti, (void*)locals);
	// don't care about the error from Deallocate, there is an error already
	return NULL;
      }
      (*env)->SetObjectArrayElement(env, variables, i, var);
      (*env)->DeleteLocalRef(env, var);
    }

    if (!live) {
      lvars = getAllLocals(env, thread, depth, location, count, locals);
      if (!lvars) {
	(*(agent->jvmti))->Deallocate(agent->jvmti, (void*)locals);
	// don't care about the error from Deallocate, there is an error already
	return NULL;
      }
    }
    tiErr = (*(agent->jvmti))->Deallocate(agent->jvmti, (void*)locals);
    if (tiErr != JVMTI_ERROR_NONE) {
      throwJvmtiException(env, "GetLocalVariableTable", tiErr);
      return NULL;
    }
    
  } else {
    variables = (*env)->NewObjectArray(env, 0, agent->LocalVariable, NULL);
    if (!variables || (*env)->ExceptionCheck(env)) return NULL;
    lvars = variables; // use the same empty array for the detached locals
  }

  tiErr = (*(agent->jvmti))->GetMethodModifiers(agent->jvmti, method, &i);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetMethodModifiers", tiErr);
    return NULL;
  }
  isStatic = (i & MOD_STATIC) != 0;

  tiErr = JVMTI_ERROR_NONE;
  if (get_locals) {
#ifdef __JVMTI_VERSION_1_2
    tiErr = (*(agent->jvmti))->GetLocalInstance(agent->jvmti, thread,
  						depth, &this);
#else
    if (isStatic) {
      this = NULL;
    } else {
      tiErr = (*(agent->jvmti))->GetLocalObject(agent->jvmti, thread,
						depth, 0, &this);
    }
#endif
  } else {
    this = NULL;
  }
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetLocalInstance", tiErr);
    return NULL;
  }

  tiErr = (*(agent->jvmti))->GetMethodDeclaringClass(agent->jvmti, method,
						     &declaringClass);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetMethodDeclaringClass", tiErr);
    (*env)->DeleteLocalRef(env, variables);
    return NULL;
  }

  reflectedMethod = (*env)->ToReflectedMethod(env, declaringClass, method, isStatic);
  (*env)->DeleteLocalRef(env, declaringClass);
  if (!reflectedMethod) {
    (*env)->DeleteLocalRef(env, variables);
    return NULL;
  }

  frame = (*env)->CallObjectMethod(env, tools, agent->createFrame,
				   thread, reflectedMethod, totalDepth-depth,
				   this, location, variables, lvars);
  (*env)->DeleteLocalRef(env, reflectedMethod);
  (*env)->DeleteLocalRef(env, variables);
  if (this) (*env)->DeleteLocalRef(env, this);
  if ((*env)->ExceptionCheck(env)) return NULL;
  return frame;
}

void JNICALL
OnFramePop(jvmtiEnv *jvmti, JNIEnv* env, jthread thread, jmethodID method,
	 jboolean was_popped_by_exception)
{
  jvmtiError tiErr;
  jint height;
  jmethodID found;
  jlocation location = -1;
  jvmtiLocalVariableEntry *locals;
  jint count;
  jobjectArray result = NULL;

  tiErr = (*jvmti)->GetFrameCount(jvmti, thread, &height);
  if (tiErr != JVMTI_ERROR_NONE) {
    logError("OnFramePop", "GetFrameCount", tiErr);
  } else {
    tiErr = (*(agent->jvmti))->GetFrameLocation(agent->jvmti, thread, 0,
                                                &found, &location);
    if (tiErr != JVMTI_ERROR_NONE || method != found) {
      logError("OnFramePop", "GetFrameLocation", tiErr);
    } else {
      tiErr = (*(agent->jvmti))->GetLocalVariableTable(agent->jvmti, method,
                                                       &count, &locals);
      if (tiErr != JVMTI_ERROR_NONE) {
	logError("OnFramePop", "GetLocalVariableTable", tiErr);
      } else {
        result = getAllLocals(env, thread, 0, location, count, locals);
	if (!result) { // There was an exception, log and swallow
	  (*env)->ExceptionDescribe(env);
	  (*env)->ExceptionClear(env);
	}
      }
    }
    (*env)->CallVoidMethod(env, agent->tools, agent->popFrame,
			   thread, height, location, result);
  }
}

jvmtiEnv *JvmToolingInterface(JavaVM *jvm) {
  int err;
  jvmtiEnv *jvmti;
  if (!jvm) {
    return NULL;
  }
#ifdef __JVMTI_VERSION_1_2
  err = (*jvm)->GetEnv(jvm, (void **)&jvmti, JVMTI_VERSION_1_2);
#else
  err = (*jvm)->GetEnv(jvm, (void **)&jvmti, JVMTI_VERSION_1_1);
#endif
  if (err != JNI_OK) {
    return NULL;
  }
  return jvmti;
}

jboolean InitializeAgent(JavaVM *jvm) {
  static AgentData data;

  jvmtiCapabilities available;
  jvmtiCapabilities request;
  jvmtiEventCallbacks callbacks;
  jvmtiError err;

  if (!agent) {
    (void)memset(&data, 0, sizeof(data));
    agent = &data;
    agent->jvmti = JvmToolingInterface(jvm);
  }
  if (agent->jvmti) {
    if (!agent->tools) {
      // Initialize capabilities
      err = (*(agent->jvmti))->GetCapabilities(agent->jvmti, &request);
      if (err == JVMTI_ERROR_NONE) {
	err = (*(agent->jvmti))->GetPotentialCapabilities(agent->jvmti,
							  &available);
	if (err == JVMTI_ERROR_NONE) {
	  
	  // <CAPABILITIES>
	  if (available.can_tag_objects)
	    request.can_tag_objects = 1;
	  if (available.can_access_local_variables)
	    request.can_access_local_variables = 1;
	  if (available.can_generate_frame_pop_events)
	    request.can_generate_frame_pop_events = 1;
	  // </CAPABILITIES>
	  
	  err = (*(agent->jvmti))->AddCapabilities(agent->jvmti, &request);
	}
      }
      if (err != JVMTI_ERROR_NONE) {
	logError("InitializeAgent","capability setup", err);
      } else {
	callbacks.FramePop = &OnFramePop;
	err = (*(agent->jvmti))->SetEventCallbacks(agent->jvmti, &callbacks,
						   sizeof(callbacks));
	if (err != JVMTI_ERROR_NONE) {
	  logError("InitializeAgent", "SetEventCallbacks", err);
	} else if (available.can_generate_frame_pop_events) {
	  err=(*(agent->jvmti))->SetEventNotificationMode(agent->jvmti,
							  JVMTI_ENABLE,
							  JVMTI_EVENT_FRAME_POP,
							  NULL);
	  if (err != JVMTI_ERROR_NONE) {
	    logError("InitializeAgent", "SetEventNotificationMode", err);
	  }
	}
      }
    }
    
    return JNI_TRUE;
  } else {
    return JNI_FALSE;
  }
}

// JVMTI Agent events

JNIEXPORT jint JNICALL 
Agent_OnLoad(JavaVM *jvm, char *options, void *reserved)
{
  if (!InitializeAgent(jvm)) {
    fprintf(stderr, "AGENT ERROR: Could not load JVM Tooling Interface.\n");
  }
  return 0;
}

// JNI SECTION

jobject getEnum(JNIEnv *env, const char *type, const char *name) {
  jclass enumClass;
  jfieldID field;
  char sign[128];
  jobject constant;

  sprintf(sign, "L%s;", type);
  enumClass = (*env)->FindClass(env, type);
  if (!enumClass) return NULL;
  field = (*env)->GetStaticFieldID(env, enumClass, name, sign);
  if (!field) return NULL;
  constant = (*env)->GetStaticObjectField(env, enumClass, field);
  (*env)->DeleteLocalRef(env, enumClass);
  if ((*env)->ExceptionCheck(env)) return NULL;
  return constant;
}

JNIEXPORT jobject JNICALL 
Java_org_thobe_java_tooling_ToolingInterface_initialize0
(JNIEnv *env, jclass tooling, jobject capabilities)
{
  int err;
  jvmtiError tiErr;
  JavaVM *jvm;
  jvmtiCapabilities available;
  jclass Set;
  jobject capability;
  jmethodID factory;
  jmethodID add;
  jobject instance;
  jint version;

  err = (*env)->GetJavaVM(env, &jvm);
  if (err != 0) {
    throwException(env, "java/lang/IllegalStateException",
		   "Could not access the JavaVM.");
    return NULL;
  }

  if (!InitializeAgent(jvm)) {
    throwException(env, "java/lang/IllegalStateException",
		   "Could not initialize the agent.");
    return NULL;
  }

  if (!agent->tools) {

    tiErr = (*(agent->jvmti))->GetCapabilities(agent->jvmti, &available);
    if (tiErr != JVMTI_ERROR_NONE) {
      throwJvmtiException(env, "GetCapabilities", tiErr);
      return NULL;
    }
    
    if (!InitializeJNI(env, tooling)) {
      return NULL;
    }
    
    Set = (*env)->FindClass(env, "java/util/Set");
    add = (*env)->GetMethodID(env, Set, "add", "(Ljava/lang/Object;)Z");
    factory = (*env)->GetMethodID(env,tooling,"<init>","(ILjava/util/Set;)V");
    
    if (available.can_tag_objects) {
      capability = getEnum(env, "org/thobe/java/tooling/Capability",
			   "can_tag_objects");
      if (!capability) return NULL;
      (*env)->CallBooleanMethod(env, capabilities, add, capability);
      (*env)->DeleteLocalRef(env, capability);
      if ((*env)->ExceptionCheck(env)) return NULL;
    }
    if (available.can_access_local_variables) {
      capability = getEnum(env, "org/thobe/java/tooling/Capability",
			   "can_access_local_variables");
      if (!capability) return NULL;
      (*env)->CallBooleanMethod(env, capabilities, add, capability);
      (*env)->DeleteLocalRef(env, capability);
      if ((*env)->ExceptionCheck(env)) return NULL;
    }
    if (available.can_generate_frame_pop_events) {
      capability = getEnum(env, "org/thobe/java/tooling/Capability",
			   "can_generate_frame_pop_events");
      if (!capability) return NULL;
      (*env)->CallBooleanMethod(env, capabilities, add, capability);
      (*env)->DeleteLocalRef(env, capability);
      if ((*env)->ExceptionCheck(env)) return NULL;
    }

    tiErr = (*(agent->jvmti))->GetVersionNumber(agent->jvmti, &version);
    if (tiErr != JVMTI_ERROR_NONE) {
      throwJvmtiException(env, "GetVersionNumber", tiErr);
      return NULL;
    }

    instance = (*env)->NewObject(env, tooling, factory, version, capabilities);
    if ((*env)->ExceptionCheck(env)) return NULL;
    agent->tools = (*env)->NewGlobalRef(env, instance);

  } else {

    factory = (*env)->GetStaticMethodID(env,tooling,"verifyPermission","()V");
    (*env)->CallStaticVoidMethod(env, tooling, factory);
    if ((*env)->ExceptionCheck(env)) return NULL;
    instance = agent->tools;

  }

  return instance;
}

JNIEXPORT jobject JNICALL 
Java_org_thobe_java_tooling_ToolingInterface_getCallFrame0
(JNIEnv *env, jobject this, jthread thread, jint depth,
 jboolean locals, jboolean live)
{
  jvmtiError tiErr;
  jvmtiFrameInfo frames[1];
  jint count;
  jboolean isNative;

  if (!verifyTool(env, this)) return NULL;

  if (live) {
    tiErr = (*(agent->jvmti))->NotifyFramePop(agent->jvmti, thread, depth);
    switch (tiErr) {
    case JVMTI_ERROR_NONE:
      break; // all is well
    case JVMTI_ERROR_OPAQUE_FRAME:
      live = JNI_FALSE; // native frames cannot be live
      break;
    case JVMTI_ERROR_ILLEGAL_ARGUMENT:
    case JVMTI_ERROR_NO_MORE_FRAMES:
      return NULL; // null returned == stack not that deep
    default:
      throwJvmtiException(env, "NotifyFramePop", tiErr);
      return NULL;
    }
  }

  tiErr = (*(agent->jvmti))->GetStackTrace(agent->jvmti, thread, depth, 1,
					   frames, &count);
  if (tiErr != JVMTI_ERROR_NONE) {
    if (tiErr != JVMTI_ERROR_ILLEGAL_ARGUMENT) {
      throwJvmtiException(env, "GetStackTrace", tiErr);
    }
    return NULL;
  }

  if (count == 0) return NULL; // null returned == stack not that deep

  tiErr = (*(agent->jvmti))->IsMethodNative(agent->jvmti, frames->method,
					    &isNative);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "IsMethodNative", tiErr);
    return NULL;
  }

  // get the current total stack depth 'count'
  tiErr = (*(agent->jvmti))->GetFrameCount(agent->jvmti, thread, &count);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetFrameCount", tiErr);
    return NULL;
  }

  return createFrame(env, this, thread, count, depth, live, 
		     frames->method, frames->location, locals, isNative);
}

jint localDepth(JNIEnv *env, jthread thread, jobject reflectMethod,
		jint height, jlong start, jint length)
{
  jvmtiError tiErr;
  jthread currentThread;
  jmethodID method;
  jvmtiFrameInfo frames[1];
  jint count;
  jint depth;

  method = (*env)->FromReflectedMethod(env, reflectMethod);

  tiErr = (*(agent->jvmti))->GetCurrentThread(agent->jvmti, &currentThread);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetCurrentThread", tiErr);
    return -1;
  }
  if (!(*env)->IsSameObject(env, thread, currentThread)) {
    // TODO: suspend the other thread instead, to ensure that depth is fixed
    throwException(env, "java/lang/UnsupportedOperationException",
		   "Can only access locals from frames on the current thread.");
    return -1;
  }

  // Compute the stack depth
  tiErr = (*(agent->jvmti))->GetFrameCount(agent->jvmti, thread, &depth);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetFrameCount", tiErr);
    return -1;
  }
  depth = depth - height;
  if (depth < 0) {
    throwException(env, "java/lang/Error",
		   "height [%d] is greater than current stack depth [%d].",
		   height, depth+height);
    return -1;
  }

  // Verify that the method at that depth matches the expected
  tiErr = (*(agent->jvmti))->GetStackTrace(agent->jvmti, thread, depth, 1,
					   frames, &count);
  if (tiErr != JVMTI_ERROR_NONE && tiErr != JVMTI_ERROR_ILLEGAL_ARGUMENT) {
    throwJvmtiException(env, "GetStackTrace", tiErr);
    return -1;
  }
  if (count == 0 || tiErr == JVMTI_ERROR_ILLEGAL_ARGUMENT) {
    throwException(env, "java/lang/Error",
		   "No stack frame available at computed depth [%d] (count=%d,tiErr=0x%x).", depth, count, tiErr);
    return -1;
  }
  if (frames[0].method != method) {
    throwException(env, "java/lang/Error", "Mismatching method identifiers.");
    return -1;
  }    

  // verify range
  if (frames[0].location < start || frames[0].location > start + length) {
    throwException(env, "org/thobe/java/tooling/LocalNotInRangeException",
		   "[%d-%d] (position in frame: %d).",
		   start, start+length, frames[0].location);
    return -1;
  }

  return depth;
}

JNIEXPORT jobject JNICALL
Java_org_thobe_java_tooling_ToolingInterface_getLocal
(JNIEnv *env, jobject this, jthread thread, jobject reflectMethod, jint height,
 jlong start, jint length, jint slot, jchar type)
{
  jint depth;

  if (!verifyTool(env, this)) return NULL;

  depth = localDepth(env, thread, reflectMethod, height, start, length);
  if (depth < 0) return NULL;

  return getLocal(env, thread, type, depth, slot);
}

void setLocal(JNIEnv *env, jthread thread, jchar type, jint depth, jint slot,
	      jobject value) {
  jvmtiError tiErr;
  jint iValue;
  jlong jValue;
  jfloat fValue;
  jdouble dValue;

  tiErr = JVMTI_ERROR_NONE;
  switch(type) {
  case '[': // array (is an Object)
  case 'L': // Object
    tiErr = (*(agent->jvmti))->SetLocalObject(agent->jvmti, thread,
					      depth, slot, value);
    if (tiErr == JVMTI_ERROR_TYPE_MISMATCH) {
      throwException(env,"java/lang/ClassCastException","");
      return;
    }
    break;
  case 'J': // long
    jValue = (*env)->CallLongMethod(env,agent->tools,agent->unboxLong,value);
    if ((*env)->ExceptionCheck(env)) return; // unboxing could throw NPE
    tiErr = (*(agent->jvmti))->SetLocalLong(agent->jvmti, thread,
					    depth, slot, jValue);
    break;
  case 'F': // float
    fValue = (*env)->CallFloatMethod(env,agent->tools,agent->unboxFloat,value);
    if ((*env)->ExceptionCheck(env)) return; // unboxing could throw NPE
    tiErr = (*(agent->jvmti))->SetLocalFloat(agent->jvmti, thread,
					     depth, slot, fValue);
    break;
  case 'D': // double
    dValue=(*env)->CallDoubleMethod(env,agent->tools,agent->unboxDouble,value);
    if ((*env)->ExceptionCheck(env)) return; // unboxing could throw NPE
    tiErr = (*(agent->jvmti))->SetLocalDouble(agent->jvmti, thread,
					      depth, slot, dValue);
    break;
  case 'I': // int
    iValue = (*env)->CallIntMethod(env,agent->tools,agent->unboxInt,value);
    goto SET_LOCAL_INT;
  case 'S': // short
    iValue = (*env)->CallShortMethod(env,agent->tools,agent->unboxShort,value);
    goto SET_LOCAL_INT;
  case 'C': // char
    iValue = (*env)->CallCharMethod(env,agent->tools,agent->unboxChar,value);
    goto SET_LOCAL_INT;
  case 'B': // byte
    iValue = (*env)->CallByteMethod(env,agent->tools,agent->unboxByte,value);
    goto SET_LOCAL_INT;
  case 'Z': // boolean
    iValue = (*env)->CallBooleanMethod(env,agent->tools,agent->unboxBool,value);
  SET_LOCAL_INT:
    if ((*env)->ExceptionCheck(env)) return; // unboxing could throw NPE
    tiErr = (*(agent->jvmti))->SetLocalInt(agent->jvmti, thread,
					   depth, slot, iValue);
    break;
  default:
    throwException(env, "java/lang/IllegalArgumentException",
		   "Invalid type identifier [%c].", type);
  }
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "SetLocal<value>", tiErr);
  }
}

JNIEXPORT void JNICALL
Java_org_thobe_java_tooling_ToolingInterface_setLocal
(JNIEnv *env, jobject this, jthread thread, jobject reflectMethod, jint height,
 jlong start, jint length, jint slot, jchar type, jobject value)
{
  jint depth;

  if (verifyTool(env, this)) {
    depth = localDepth(env, thread, reflectMethod, height, start, length);
    if (depth >= 0) {
      setLocal(env, thread, type, depth, slot, value);
    }
  }
}

jvmtiIterationControl JNICALL tag_object(jlong class_tag, jlong size,
					 jlong* tag_ptr, void* user_data) {
  *tag_ptr = *((jlong*)user_data);
  return JVMTI_ITERATION_CONTINUE;
}

JNIEXPORT jobjectArray JNICALL 
Java_org_thobe_java_tooling_ToolingInterface_getLiveInstances0
(JNIEnv *env, jobject this, jclass type, jlong tag)
{
  jvmtiError tiErr;
  jint count;
  jint i;
  jobject *objects;
  jlong *tags;
  jobjectArray result;

  if (!verifyTool(env, this)) return NULL;

  tiErr=(*(agent->jvmti))->IterateOverInstancesOfClass(agent->jvmti, type,
						       JVMTI_HEAP_OBJECT_EITHER,
						       &tag_object, &tag);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "IterateOverInstancesOfClass", tiErr);
    return NULL;
  }

  tiErr=(*(agent->jvmti))->GetObjectsWithTags(agent->jvmti, 1, &tag, &count,
					      &objects,&tags);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetObjectsWithTags", tiErr);
    return NULL;
  }
  (*(agent->jvmti))->Deallocate(agent->jvmti, (void*)tags);

  result = (*env)->NewObjectArray(env, count, type, NULL);
  for (i=0; i < count; i++) {
    (*(agent->jvmti))->SetTag(agent->jvmti, objects[i], 0);
    (*env)->SetObjectArrayElement(env, result, i, objects[i]);
  }

  (*(agent->jvmti))->Deallocate(agent->jvmti, (void*)objects);

  return result;
}

JNIEXPORT jlong JNICALL 
Java_org_thobe_java_tooling_ToolingInterface_sizeOf
(JNIEnv *env, jobject this, jobject object)
{
  jvmtiError tiErr;
  jlong size;
  if (!verifyTool(env, this) || !object) return 0;
  tiErr = (*(agent->jvmti))->GetObjectSize(agent->jvmti, object, &size);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetObjectSize", tiErr);
    return 0;
  }
  return size;
}

jvmtiIterationControl JNICALL sum_size(jvmtiObjectReferenceKind reference_kind, 
				       jlong class_tag, jlong size, 
				       jlong* tag_ptr, jlong referrer_tag, 
				       jint referrer_index, void* user_data) {
  if (reference_kind == JVMTI_REFERENCE_CLASS) {
    return JVMTI_ITERATION_IGNORE;
  }
  *((jlong*)user_data) += size;
  return JVMTI_ITERATION_CONTINUE;
}

JNIEXPORT jlong JNICALL 
Java_org_thobe_java_tooling_ToolingInterface_retainedSize0
(JNIEnv *env, jobject this, jobject object)
{
  jvmtiError tiErr;
  jlong size;
  if (!verifyTool(env, this) || !object) return 0;
  tiErr = (*(agent->jvmti))->GetObjectSize(agent->jvmti, object, &size);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "GetObjectSize", tiErr);
    return 0;
  }
  tiErr = (*(agent->jvmti))->IterateOverObjectsReachableFromObject(agent->jvmti,
								   object,
								   &sum_size,
								   &size);
  if (tiErr != JVMTI_ERROR_NONE) {
    throwJvmtiException(env, "IterateOverObjectsReachableFromObject", tiErr);
    return 0;
  }
  return size;
}
