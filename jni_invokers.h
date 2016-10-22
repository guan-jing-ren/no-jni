template<typename R>
struct invoke_jni;

template<>
struct invoke_jni<jvoid>
{
  template<typename... Args>
  static jvoid call(const jobject& obj, jmethodID method, Args&&... args)
  {
    ::jni->CallVoidMethod(obj, method, args...);
    return jvoid_arg;
  }

  template<typename... Args>
  static jvoid call(const jclass& klass, jmethodID method, Args&&... args)
  {
    ::jni->CallStaticVoidMethod(klass, method, args...);
    return jvoid_arg;
  }
};

template<>
struct invoke_jni<jobject>
{
  template<typename... Args>
  static jobject call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallObjectMethod(obj, method, args...);
  }

  template<typename... Args>
  static jobject call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticObjectMethod(klass, method, args...);
  }

  static jobject get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetObjectField(obj, field);
  }

  static jobject get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticObjectField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, const jobject& value)
  {
    ::jni->SetObjectField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, const jobject& value)
  {
    ::jni->SetStaticObjectField(klass, field, value);
  }

  using array_type = jobjectArray;
  static jobjectArray array(jsize length, const jclass& klass, const jobject& init = nullptr)
  {
    return ::jni->NewObjectArray(length, klass, init);
  }

  static std::vector<jobject> to_vector(array_type array)
  {
    auto len = static_cast<std::vector<jobject>::size_type>(::jni->GetArrayLength(array));
    auto elems = std::vector<jobject>{len};
    for(auto end = len, i = decltype(len){0}; i < end; ++i)
    {
      elems.emplace_back(get(array, i));
    }
    return elems;
  }

  static jobject get(array_type array, jsize index)
  {
    return ::jni->GetObjectArrayElement(array, index);
  }

  static void set(array_type array, const jobject& elem, jsize index)
  {
    ::jni->SetObjectArrayElement(array, index, elem);
  }
};

template<>
struct invoke_jni<jboolean>
{
  template<typename... Args>
  static jboolean call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallBooleanMethod(obj, method, args...);
  }

  template<typename... Args>
  static jboolean call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticBooleanMethod(klass, method, args...);
  }

  static jboolean get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetBooleanField(obj, field);
  }

  static jboolean get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticBooleanField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, jboolean value)
  {
    ::jni->SetBooleanField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, jboolean value)
  {
    ::jni->SetStaticBooleanField(klass, field, value);
  }

  using array_type = jbooleanArray;
  static jbooleanArray array(jsize length)
  {
    return ::jni->NewBooleanArray(length);
  }

  static std::vector<jboolean> to_vector(array_type array)
  {
    auto len = ::jni->GetArrayLength(array);
    auto elems = std::make_unique<jboolean>(len);
    ::jni->GetBooleanArrayRegion(array, 0, len, elems.get());
    return {elems.get(), elems.get() + len};
  }

  static jboolean get(array_type array, jsize index, jsize len = 1)
  {
    jboolean elem;
    ::jni->GetBooleanArrayRegion(array, index, len, &elem);
    return elem;
  }

  static void set(array_type array, jboolean elem, jsize index, jsize len = 1)
  {
    ::jni->SetBooleanArrayRegion(array, index, len, &elem);
  }
};

template<>
struct invoke_jni<jbyte>
{
  template<typename... Args>
  static jbyte call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallByteMethod(obj, method, args...);
  }

  template<typename... Args>
  static jbyte call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticByteMethod(klass, method, args...);
  }

  static jbyte get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetByteField(obj, field);
  }

  static jbyte get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticByteField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, jbyte value)
  {
    ::jni->SetByteField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, jbyte value)
  {
    ::jni->SetStaticByteField(klass, field, value);
  }

  using array_type = jbyteArray;
  static jbyteArray array(jsize length)
  {
    return ::jni->NewByteArray(length);
  }

  static std::vector<jbyte> to_vector(array_type array)
  {
    auto len = ::jni->GetArrayLength(array);
    auto elems = std::make_unique<jbyte>(len);
    ::jni->GetByteArrayRegion(array, 0, len, elems.get());
    return {elems.get(), elems.get() + len};
  }

  static jbyte get(array_type array, jsize index, jsize len = 1)
  {
    jbyte elem;
    ::jni->GetByteArrayRegion(array, index, len, &elem);
    return elem;
  }

  static void set(array_type array, jbyte elem, jsize index, jsize len = 1)
  {
    ::jni->SetByteArrayRegion(array, index, len, &elem);
  }
};

template<>
struct invoke_jni<jchar>
{
  template<typename... Args>
  static jchar call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallCharMethod(obj, method, args...);
  }

  template<typename... Args>
  static jchar call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticCharMethod(klass, method, args...);
  }

  static jchar get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetCharField(obj, field);
  }

  static jchar get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticCharField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, jchar value)
  {
    ::jni->SetCharField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, jchar value)
  {
    ::jni->SetStaticCharField(klass, field, value);
  }

  using array_type = jcharArray;
  static jcharArray array(jsize length)
  {
    return ::jni->NewCharArray(length);
  }

  static std::vector<jchar> to_vector(array_type array)
  {
    auto len = ::jni->GetArrayLength(array);
    auto elems = std::make_unique<jchar>(len);
    ::jni->GetCharArrayRegion(array, 0, len, elems.get());
    return {elems.get(), elems.get() + len};
  }

  static jchar get(array_type array, jsize index, jsize len = 1)
  {
    jchar elem;
    ::jni->GetCharArrayRegion(array, index, len, &elem);
    return elem;
  }

  static void set(array_type array, jchar elem, jsize index, jsize len = 1)
  {
    ::jni->SetCharArrayRegion(array, index, len, &elem);
  }
};

template<>
struct invoke_jni<jshort>
{
  template<typename... Args>
  static jshort call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallShortMethod(obj, method, args...);
  }

  template<typename... Args>
  static jshort call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticShortMethod(klass, method, args...);
  }

  static jshort get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetShortField(obj, field);
  }

  static jshort get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticShortField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, jshort value)
  {
    ::jni->SetShortField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, jshort value)
  {
    ::jni->SetStaticShortField(klass, field, value);
  }

  using array_type = jshortArray;
  static jshortArray array(jsize length)
  {
    return ::jni->NewShortArray(length);
  }

  static std::vector<jshort> to_vector(array_type array)
  {
    auto len = ::jni->GetArrayLength(array);
    auto elems = std::make_unique<jshort>(len);
    ::jni->GetShortArrayRegion(array, 0, len, elems.get());
    return {elems.get(), elems.get() + len};
  }

  static jshort get(array_type array, jsize index, jsize len = 1)
  {
    jshort elem;
    ::jni->GetShortArrayRegion(array, index, len, &elem);
    return elem;
  }

  static void set(array_type array, jshort elem, jsize index, jsize len = 1)
  {
    ::jni->SetShortArrayRegion(array, index, len, &elem);
  }
};

template<>
struct invoke_jni<jint>
{
  template<typename... Args>
  static jint call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallIntMethod(obj, method, args...);
  }

  template<typename... Args>
  static jint call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticIntMethod(klass, method, args...);
  }

  static jint get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetIntField(obj, field);
  }

  static jint get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticIntField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, jint value)
  {
    ::jni->SetIntField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, jint value)
  {
    ::jni->SetStaticIntField(klass, field, value);
  }

  using array_type = jintArray;
  static jintArray array(jsize length)
  {
    return ::jni->NewIntArray(length);
  }

  static std::vector<jint> to_vector(array_type array)
  {
    auto len = ::jni->GetArrayLength(array);
    auto elems = std::make_unique<jint>(len);
    ::jni->GetIntArrayRegion(array, 0, len, elems.get());
    return {elems.get(), elems.get() + len};
  }

  static jint get(array_type array, jsize index, jsize len = 1)
  {
    jint elem;
    ::jni->GetIntArrayRegion(array, index, len, &elem);
    return elem;
  }

  static void set(array_type array, jint elem, jsize index, jsize len = 1)
  {
    ::jni->SetIntArrayRegion(array, index, len, &elem);
  }
};

template<>
struct invoke_jni<jlong>
{
  template<typename... Args>
  static jlong call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallLongMethod(obj, method, args...);
  }

  template<typename... Args>
  static jlong call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticLongMethod(klass, method, args...);
  }

  static jlong get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetLongField(obj, field);
  }

  static jlong get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticLongField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, jlong value)
  {
    ::jni->SetLongField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, jlong value)
  {
    ::jni->SetStaticLongField(klass, field, value);
  }

  using array_type = jlongArray;
  static jlongArray array(jsize length)
  {
    return ::jni->NewLongArray(length);
  }

  static std::vector<jlong> to_vector(array_type array)
  {
    auto len = ::jni->GetArrayLength(array);
    auto elems = std::make_unique<jlong>(len);
    ::jni->GetLongArrayRegion(array, 0, len, elems.get());
    return {elems.get(), elems.get() + len};
  }

  static jlong get(array_type array, jsize index, jsize len = 1)
  {
    jlong elem;
    ::jni->GetLongArrayRegion(array, index, len, &elem);
    return elem;
  }

  static void set(array_type array, jlong elem, jsize index, jsize len = 1)
  {
    ::jni->SetLongArrayRegion(array, index, len, &elem);
  }
};

template<>
struct invoke_jni<jfloat>
{
  template<typename... Args>
  static jfloat call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallFloatMethod(obj, method, args...);
  }

  template<typename... Args>
  static jfloat call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticFloatMethod(klass, method, args...);
  }

  static jfloat get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetFloatField(obj, field);
  }

  static jfloat get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticFloatField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, jfloat value)
  {
    ::jni->SetFloatField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, jfloat value)
  {
    ::jni->SetStaticFloatField(klass, field, value);
  }

  using array_type = jfloatArray;
  static jfloatArray array(jsize length)
  {
    return ::jni->NewFloatArray(length);
  }

  static std::vector<jfloat> to_vector(array_type array)
  {
    auto len = ::jni->GetArrayLength(array);
    auto elems = std::make_unique<jfloat>(len);
    ::jni->GetFloatArrayRegion(array, 0, len, elems.get());
    return {elems.get(), elems.get() + len};
  }

  static jfloat get(array_type array, jsize index, jsize len = 1)
  {
    jfloat elem;
    ::jni->GetFloatArrayRegion(array, index, len, &elem);
    return elem;
  }

  static void set(array_type array, jfloat elem, jsize index, jsize len = 1)
  {
    ::jni->SetFloatArrayRegion(array, index, len, &elem);
  }
};

template<>
struct invoke_jni<jdouble>
{
  template<typename... Args>
  static jdouble call(const jobject& obj, jmethodID method, Args&&... args)
  {
    return ::jni->CallDoubleMethod(obj, method, args...);
  }

  template<typename... Args>
  static jdouble call(const jclass& klass, jmethodID method, Args&&... args)
  {
    return ::jni->CallStaticDoubleMethod(klass, method, args...);
  }

  static jdouble get(const jobject& obj, jfieldID field)
  {
    return ::jni->GetDoubleField(obj, field);
  }

  static jdouble get(const jclass& klass, jfieldID field)
  {
    return ::jni->GetStaticDoubleField(klass, field);
  }

  static void set(const jobject& obj, jfieldID field, jdouble value)
  {
    ::jni->SetDoubleField(obj, field, value);
  }

  static void set(const jclass& klass, jfieldID field, jdouble value)
  {
    ::jni->SetStaticDoubleField(klass, field, value);
  }

  using array_type = jdoubleArray;
  static jdoubleArray array(jsize length)
  {
    return ::jni->NewDoubleArray(length);
  }

  static std::vector<jdouble> to_vector(array_type array)
  {
    auto len = ::jni->GetArrayLength(array);
    auto elems = std::make_unique<jdouble>(len);
    ::jni->GetDoubleArrayRegion(array, 0, len, elems.get());
    return {elems.get(), elems.get() + len};
  }

  static jdouble get(array_type array, jsize index, jsize len = 1)
  {
    jdouble elem;
    ::jni->GetDoubleArrayRegion(array, index, len, &elem);
    return elem;
  }

  static void set(array_type array, jdouble elem, jsize index, jsize len = 1)
  {
    ::jni->SetDoubleArrayRegion(array, index, len, &elem);
  }
};
