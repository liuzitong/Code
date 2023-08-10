【 ScVector 】

设计特性：
    1) 基于 ScMemBlock 设计，支持全部 ScMemBlock 的特性

    2) 使用模板方式设计，因此支持基本类型 ( fix_step < 2 ), 以及自定义类型 ( fix_step >= 2 )
       自定义类至少需要实现 复制构造 或者 赋值复制
   
    3) 使用[] 下标进行访问时，需要特别注意C++ 的const 特性。
       3-1) 默认情况下，[] 将调用  T &  operator[]( int i ) 的实现，此实现将每次检测
            是否产生了共享，如果已共享数据，则将执行复制函数复制数据，以试图满足使用者修改成员内容的意图。

       3-2) 若是使用者意图为只读取数据，按下述方式使用将不检测是否共享
            const ScVector<T> &vec_const = vec;  // vec 为数据集
            T & val = vec_const[ i ];            // 此时将调用  const T& operator[]( int ) const 的实现
            const T &val2 = vec_const[i];    

       * 为确保只读访问，请使用 ScVector::at( size_t idx ) 方法       
       
API 说明：     
     ScVector<T>()
     描述：构造一个空的对象

     ScVector<T>( size_t dim )
     描述：构造一个指定大小空间的对象
     参数：dim [IN] -- 一维大小，即需要的空间大小
     
     ScVector<T>( const ScVector<T> &ot )
     描述：复制构造指定的对象。当前向量对象将与指定对象共享同样的数据 
     参数：ot [IN] -- 指定的对象
     注意：执行此操作后，当前对象将与 'ot' 共享相同的数据

     ScVector<T>& operator = ( const ScVector<T> &ot )
     描述：放弃自身的数据（如果有），联接到指定的对象的数据。
     参数：ot [IN] -- 指定的对象
     注意：执行此操作后，当前向量对象与 'ot' 共享同一片数据。

     bool     isNull() const
     描述：判断当前对象是否有效。
     返回：true 代表对象无效。

     size_t   dim() const
     描述：返回构造这个对象时传入的一维大小数值
     
     const T* constData()
     描述：返回只读的数据首地址
     返回：只读指针，如果当前对象无效，则返回 nullptr
     警告：如果修改数据，则可能影响其他共享数据的对象
     
     bool     dupIfShared()
     描述：判断当前对象是否与其他对象共享一片数据空间，如果是，则复制数据，否则不做任何事
     返回：true 复制了数据，false 没有复制
     注意：如果实例化的T 为自定义数据类，则可能产生大量的数据构造复制等操作。

     T*       dataCow()     
     描述：判断当前对象是否与其他对象共享一片数据空间，如果是，则复制数据，同时返回可写数据指针
     返回：数据指针。如果对象为无效状态，则返回 nullptr

     ScVector<T> copy() const
     描述：直接复制一份数据并返回
     注意：如果当前对象无效，也返回无效的对象

     template <typename Ti>
     const T&  operator[]( const Ti &i ) const
     描述：以下标访问方式返回指定索引的数据引用地址
     注意：下标不可以超过最大容量（即 dim()）
     变更：fix_step < 2 此函数是非模板函数，而 fix_step > 2 则改为模板函数 
    
     template <typename Ti>
     T&  operator[]( const Ti &i )
     下标的可修改数据版本。
     
     void    makeNull()
     版本：fix_step == 2 引入
     描述：强制释放数据
     注意：如果该描述符与其他描述符共享了数据，则此操作只是断开共享的数据。

     size_t  size() const
     版本：fix_step == 2 引入
     描述：返回当前的有效数据个数
     注意：在 fix_step < 2 时，此值与 dim() 相同。fix_step >= 2 时，将作为以后支持动态元素的一个
     条件，用于返回当前有效的元素个数（以后添加的 push_back ）

     size_t  capacity() const
     版本：fix_step == 2 引入
     描述：返回容器最大容量

     void   push_back( const T& v )
     版本：fix_step == 3 引入
     描述：往容器中压入一个元素，如果容器太小或者为空，则空间自动增长以满足要求。

     void   pop_back()
     版本：fix_step == 3 引入
     描述：从容器尾部删除一个元素

     void   push_front( const T& v )
     版本：fix_step == 3 引入
     描述：在最前方压入一个元素
     警告：当前所有其他元素均往后移一个空位！将产生大量数据移动。
     注意：如果类型为 class，则务必实现 移动赋值 函数以实现较低的数据移动成本！    

     void   pop_front()
     版本：fix_step == 3 引入
     描述：从最前方删除一个元素          
     
     bool   empty() const
     版本：fix_step == 3 引入
     描述：判断容器是否为空
     返回：true 为真

     size_t  max_size() const
     版本：fix_step == 3 引入
     描述：兼容，只是返回一个最大值

     void    clear()
     版本：fix_step == 3 引入
     描述：清空容器中的项目

     void    reserve( size_t n )
     版本：fix_step == 3 引入
     描述：预保留空间，以便于执行成员添加时少动态分配内存

     void    resize( size_t n )
     版本：fix_step == 3 引入
     描述：重新扩展元素到 n。如果容器中元素少于 n，将默认补足元素。
           如果元素数目多于 n, 将释放多余的元素 
     
     void    insert( size_t pos, const T&v )
     版本：fix_step == 3 引入
     描述：在指定位置插入元素。如：1,3,5  在下标 1 上插入9 将变成 1,9,3,5
      
     void    remove( size_t pos )
     版本：fix_step == 3 引入
     描述：移除指定位置上的元素。如：1,3,5 移除下标 1 的元素，将变成 1,5

     const T&   back() const
     版本：fix_step == 3 引入
     描述：返回容器最末尾的只读元素

     const T&   front() const
     版本：fix_step == 3 引入
     描述：返回容器最前方的只读元素

     const T&   at( size_t pos )
     版本：fix_step == 3 引入
     描述：返回指定下标的元素只读引用
     
    
     