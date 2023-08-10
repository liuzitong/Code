【 ScMemBlock 】

设计特性：
   1) 可用于在不同 C++ 运行库堆中传递（引用，指针，复制等等方式），保证各个模组链接不同运行库时安全。

   2）保证前向版本兼容性。
      如 ScMemBlock 0.1.0 的内存块可以在 ScMemBlock 0.2.0 后续版本安全释放, 反之亦可。

   3) 实现上为共享读，可选写。
      ScMemBlock 构造复制，赋值复制采取隐式共享方式。使用者可以简单地将其像变量一样传递到需要使用内
      存的自定义库中。

   4) 支持手动方式的共享复制。

   5) 支持返回数据指针时判断复制。

   6) 支持跨线程传递。
      警告：不保证线程读写安全性，但保证共享读安全。线程之间传递时必须以值方式传递，不能以指针方式传递！

   *) 不用担心内存释放，在内部引用计数归零时，自动释放。
      内存块尾部追加校验字节，可检测到写数据时超出边界的一些情况。

API 说明：

    ScMemBlock()
    描述: 构建一个内存块描述符，该描述符指向空地址 ( nullptr )

    ScMemBlock( size_t sz )
    描述：构建一个指定大小的内存块描述符
    参数：sz -- 字节尺寸，如果 < 1, 则等同于 ScMemBlock() 
    
    ScMemBlock( size_t sz, bool ( *UserProc )( ActID act_id, void *act_param, void* ), void* )
    版本：0.2.0 引入
    描述：构建一个指定大小的内存块，并且挂接回调函数来自定义处理内存块的活动
    参数：sz -- 字节尺寸，如果 < 1，则等同于 ScMemBlock()
          UserProc -- 回调函数，如果为 nullptr, 则使用默认处理例程
    相关：
          ActID 行为枚举值
          ActID_Init   : 当内存分配后调用  （如填0 等操作）时回调，act_param 传回 struct ActInitParam 结构指针
          ActID_Destroy: 当内存在释放前调用, act_param 传回 struct ActDestroyParam 结构指针
          ActID_InitByCopy : 当内存产生复制时调用，调用者应该负责执行内容从一处复制到另一处的行为。act_param 传回 struct ActInitByCopyParam 指针
          
          struct ActInitParam {   
              void    *data;    // 数据指针
              sizez_t  data_sz; // 数据字节数
          };

          struct ActDestroyParam {
              void    *data;    // 数据指针
              size_t   data_sz; // 数据字节数
          };          

          struct ActInitByCopyParam {
              void   *tgt_data; // 复制到的目标地址
              size_t  tgt_sz;   // 目标地址有效字节数
              void   *src_data; // 复制的来源地址
              size_t  src_sz;   // 来源地址的有效字节数
          };

          * 注意：tgt_sz > src_sz 时，注意 tgt_data 没有数据可复制的部分是否需要初始化！

    ScMemBlock( const ScMemBlock &ot )
    描述：复制构建指定的内存描述符。当前描述符将与指定的描述符共享内存。
    参数：ot -- 指定的内存描述符
    注意：执行此操作后，当前描述符与 'ot' 共享同一片内存。

    ScMemBlock &  operator = ( const ScMemBlock &ot )
    描述：放弃自身的内存块（如果有），联接到指定的描述符所指向的内存。
    参数：ot -- 指定的内存描述符
    注意：执行此操作后，当前描述符与 'ot' 共享同一片内存。

    void  makeNull()
    版本：0.1.0 引入
    描述：强制释放该描述指向的内存，让该描述符指向空地址
    注意：如果该描述符与其他描述符共享了内存，则此操作只是断开共享的内存。
    相关：此操作等同于如下调用：
          ScMemBlock mb(100); mb = ScMemBlock();
    
    bool  isNull() const
    描述：判断当前描述符是否有效。无效指内存描述符指向空地址
    返回：true 代表当前描述符有效，指向有效的内存地址。

    size_t  size() const
    返回：当前描述符所指向的内存大小
    注意：如果内存描述符无效，isNull() 返回 true，size() 返回 0

    void    resize( size_t sz )
    版本：0.2.0 引入
    描述：重新变更内存描述符内存大小
    参数：sz -- 期望的大小
    注意：如果当前内存描述符为无效, 则调用此函数后, 将拥有指定大小的内存块。
          如果当前内存描述符与其他内存描述符共享同一片内存，则当前内存描述符重新分配 sz 大小空间，
          同时复制原有内存描述符中的内容。（即不会改变共享的内容）
          
    const void*  constData() const
    描述：直接返回内存地址。如果当前描述符为无效，则返回 nullptr
    返回：内存地址
    警告：如果当前描述符与其他共享内存，则修改内存内容将影响其他描述符！
    
    ScMemBlock  copy() const
    返回：复制当前的内存，返回该内存的描述符
    注意：如果当前内存描述符无效，则返回无效的内存描述符。

    bool        dupIfShared()
    描述：如果当前描述符与其他描述符共享了内存，则执行复制操作，让当前内存描述符成为独享内存描述符
    返回：true  -- 发生了复制，  当前描述符已复制一份内存
          false -- 没有发生复制，当前描述符本为独享

    void*       dataCow() 
    版本：0.2.0 引入
    描述：返回一个可写的数据指针，并且保证数据为独享。当不为独享方式时，采取 dupIfShared() 相同的方式进行复制。
    返回：可写的数据指针
    注意：必须了解到，执行此函数后，可能会生成数据复制！   

    void        swap( ScMemBlock &ot )
    版本：0.3.0 引入
    描述：与指写的对象交换数据    

使用示例：
   [ 构建空的内存块 ]
       QxPack::ScMemBlock mb();
       bool is_valid = mb.isNull();    // is_valid 为 true

   [ 构建指定大小的内存块 ]
       QxPack::ScMemBlock mb(100);     // 构建了100个字节的空间
        
   [ 附加到其他内存块上，共享其他块的空间 ]
       QxPack::ScMemBlock mb(100);
       QxPack::ScMemBlock mb2( mb );   // 通过复制构造方式联接到其他内存块
       QxPack::ScMemBlock mb3 = mb;    // 通过赋值方式联接到其他内存块

   [ 强制放弃内存 ]
       QxPack::ScMemBlock mb(100);    
       mb.makeNull();                  // 强制放弃内存
       bool is_valid = mb.isNull();    // is_valid 为 true

   [ 显式复制指定的内存块 ]
       QxPack::ScMemBlock mb(100);
       QxPack::ScMemBLock mb2 = mb.copy(); // 显式复制，mb2 指向的空间与 mb 的空间是不一样的。
        
   [ 判断共享时才复制  ]
       QxPack::ScMemBlock mb(100);
       bool is_dup = mb.dupIfShared();     // is_dup 为 false, 什么也没有发生，因为 mb 没有与其他块共享空间

       QxPack::ScMemBlock mb2 = mb;        // mb2 与 mb 指向同一片空间
       QxPack::ScMemBlock mb3 = mb;        // mb,mb2 指向同一片空间
       mb3.dupIfShared();                  // 执行此句， mb3 指向不同的空间。

   [ 取只读数据指针 ]
       QxPack::ScMemBlock mb(100);
       const void* cptr = mb.constData();  // cptr 就是数据空间地址
        
   [ 取可写数据指针，如果共享情况下复制内容后再取指针 ]
       QxPack::ScMemBlock mb(100);
       void *dp  = mb.dataCow();           // mb 没有发生复制，因为 mb 指向的空间只有一个引用
       
       QxPack::ScMemBlock mb2 = mb;
       void *dp2 = mb.dataCow();           // mb 发生了复制，因为 mb2 与 mb 共同指向同一片空间

   [ 作为函数参数的设计 ]
       
       // 以下函数演示只读参数，修改复制，返回内存结果 
       QxPack::ScMemBlock  foo( const QxPack::ScMemBlock &mb ) 
       {
           printf("mem block size:%u\n", mb.size());
        
           QxPack::ScMemBlock mb2 = mb; // 先附加到该内存块，可共享读

           const uint8_t *cdp = reinterpret_cast<const uint8_t*>( mb2.constData());
           .....
           
           uint8_t *dp = reinterpret_cast<uint8_t*>( mb2.dataCow()); // 复制一份
           dp[0] = 0xaa; // 修改内容
           
           return mb2;   // 返回修改过的结果 
       }       

       int main( int argc, void** argv ) 
       {
           QxPack::ScMemBlock mb( 100 );

           QxPack::ScMemBlock mb2 = foo( mb ); // 取得结果内存块

           return 0;
       }

   
   
