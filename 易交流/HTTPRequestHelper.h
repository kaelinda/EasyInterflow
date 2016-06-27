//
//  HTTPRequestHelper.h
//  Weather
//
//  Created by HTTP on 16/5/26.
//  Copyright © 2016年 Scott Sherwood. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
// 项目打包上线都不会打印日志，因此可放心。
#ifdef DEBUG
#define KaelAppLog(s, ... ) NSLog( @"[%@ in line %d] ===============>%@", [[NSString stringWithUTF8String:__FILE__] lastPathComponent], __LINE__, [NSString stringWithFormat:(s), ##__VA_ARGS__] )
#else
#define KaelAppLog(s, ... )
#endif

/**
 *  @author Kael
 *
 *  @brief 下载进度
 *  @param bytesRead      已下载的大小
 *  @param totalBytesRead 文件总大小
 */
typedef void (^HTTPDownloadProgress)(int64_t bytesRead,
                                    int64_t totalBytesRead);

typedef HTTPDownloadProgress HYBGetProgress;
typedef HTTPDownloadProgress HYBPostProgress;


/**
 *  @author Kael
 *
 *  @brief 上传进度
 *  @param bytesWritten      已经上传的大小
 *  @param totalBytesWritten 上传的总大小
 */
typedef void (^HTTPUploadProgress)(int64_t bytesWritten,
                                  int64_t totalBytesWritten);

typedef NS_ENUM(NSUInteger, HTTPResponseType) {
    kHTTPResponseTypeJSON = 1,//默认为json格式
    kHTTPResponseTypeXML  = 2,//XML格式
    // 特殊情况下，一转换服务器就无法识别的，默认会尝试转换成JSON，若失败则需要自己去转换
    kHTTPResponseTypeData = 3,
};

typedef NS_ENUM(NSUInteger, HTTPRequestType) {
    kHTTPRequestTypeJSON = 1,//默认
    kHTTPRequestTypePlainText  = 2 // 普通text/html
};


typedef NS_ENUM(NSUInteger, HTTPNetworkStatus) {
    kHTTPNetworkStatusUnknown          = 99,//未知网络
    kHTTPNetworkStatusNotReachable     = 0,//网络无连接
    kHTTPNetworkStatusReachableViaWWAN = 1,//2，3，4G网络 蜂窝网络
    kHTTPNetworkStatusReachableViaWiFi = 2,//WIFI网络
};


@class NSURLSessionDataTask;

// 请勿直接使用NSURLSessionDataTask,以减少对第三方的依赖
// 所有接口返回的类型都是基类NSURLSessionTask，若要接收返回值
// 且处理，请转换成对应的子类类型
typedef NSURLSessionTask HTTPSessionTask;
typedef void(^HTTPResponseSuccess)(id response);
typedef void(^HTTPResponseFail)(NSError *error);



@interface HTTPRequestHelper : NSObject


/**
 *  @author
 *
 *  @brief 用于指定网络请求接口的基础url，如：
 *  http://henishuo.com或者http://101.200.209.244
 *  通常在AppDelegate中启动时就设置一次就可以了。如果接口有来源
 *  于多个服务器，可以调用更新 *  @param baseUrl 网络接口的基础url
 *
 *  @param baseUrl 网络接口的基础url
 *
 */
+ (void)updateBaseUrl:(NSString *)baseUrl;
+ (NSString *)baseURL;
/**
 *	设置请求超时时间，默认为60秒
 *
 *	@param timeout 超时时间
 */
+ (void)setTimeout:(NSTimeInterval)timeout;

/**
 *	当检查到网络异常时，是否从从本地提取数据。默认为NO。一旦设置为YES,当设置刷新缓存时，
 *  若网络异常也会从缓存中读取数据。同样，如果设置超时不回调，同样也会在网络异常时回调，除非
 *  本地没有数据！
 *
 *	@param shouldObtain	YES/NO
 */
+ (void)obtainDataFromLocalWhenNetworkUnconnected:(BOOL)shouldObtain;


/**
 *	@author Kael
 *
 *	默认只缓存GET请求的数据，对于POST请求是不缓存的。如果要缓存POST获取的数据，需要手动调用设置
 *  对JSON类型数据有效，对于PLIST、XML不确定！
 *
 *	@param isCacheGet			默认为YES
 *	@param shouldCachePost	默认为NO
 */
+ (void)cacheGetRequest:(BOOL)isCacheGet shoulCachePost:(BOOL)shouldCachePost;


/**
 *	@author Kael
 *
 *	获取缓存总大小/bytes
 *
 *	@return 缓存大小
 */
+ (unsigned long long)totalCacheSize;

/**
 *	@author Kael
 *
 *	清除缓存
 */
+ (void)clearCaches;

/*!
 *  @author Kael
 *
 *  开启或关闭接口打印信息
 *
 *  @param isDebug 开发期，最好打开，默认是NO
 */
+ (void)enableInterfaceDebug:(BOOL)isDebug;

/*!
 *  @author Kael
 *
 *  配置请求格式，默认为JSON。如果要求传XML或者PLIST，请在全局配置一下
 *
 *  @param requestType 请求格式，默认为JSON
 *  @param responseType 响应格式，默认为JSO，
 *  @param shouldAutoEncode YES or NO,默认为NO，是否自动encode url
 *  @param shouldCallbackOnCancelRequest 当取消请求时，是否要回调，默认为YES
 */
+ (void)configRequestType:(HTTPRequestType)requestType
             responseType:(HTTPResponseType)responseType
      shouldAutoEncodeUrl:(BOOL)shouldAutoEncode
  callbackOnCancelRequest:(BOOL)shouldCallbackOnCancelRequest;

/*!
 *  @author Kael
 *
 *  配置公共的请求头，只调用一次即可，通常放在应用启动的时候配置就可以了
 *
 *  @param httpHeaders 只需要将与服务器商定的固定参数设置即可
 */
+ (void)configCommonHttpHeaders:(NSDictionary *)httpHeaders;

/**
 *	@author Kael
 *
 *	取消所有请求
 */
+ (void)cancelAllRequest;
/**
 *	@author Kael
 *
 *	取消某个请求。如果是要取消某个请求，最好是引用接口所返回来的HTTPSessionTask对象，
 *  然后调用对象的cancel方法。如果不想引用对象，这里额外提供了一种方法来实现取消某个请求
 *
 *	@param url				URL，可以是绝对URL，也可以是path（也就是不包括baseurl）
 */
+ (void)cancelRequestWithURL:(NSString *)url;



/*!
 *  @author Kael
 *
 *  GET请求接口，若不指定baseurl，可传完整的url
 *
 *  @param url     接口路径，如/path/getArticleList
 *  @param refreshCache 是否刷新缓存。由于请求成功也可能没有数据，对于业务失败，只能通过人为手动判断
 *  @param params  接口中所需要的拼接参数，如@{"categoryid" : @(12)}
 *  @param success 接口成功请求到数据的回调
 *  @param fail    接口请求数据失败的回调
 *
 *  @return 返回的对象中有可取消请求的API
 */
+ (HTTPSessionTask *)getWithUrl:(NSString *)url
                     refreshCache:(BOOL)refreshCache
                          success:(HTTPResponseSuccess)success
                             fail:(HTTPResponseFail)fail;
// 多一个params参数
+ (HTTPSessionTask *)getWithUrl:(NSString *)url
                     refreshCache:(BOOL)refreshCache
                           params:(NSDictionary *)params
                          success:(HTTPResponseSuccess)success
                             fail:(HTTPResponseFail)fail;
// 多一个带进度回调
+ (HTTPSessionTask *)getWithUrl:(NSString *)url
                     refreshCache:(BOOL)refreshCache
                           params:(NSDictionary *)params
                         progress:(HYBGetProgress)progress
                          success:(HTTPResponseSuccess)success
                             fail:(HTTPResponseFail)fail;

/*!
 *  @author Kael
 *
 *  POST请求接口，若不指定baseurl，可传完整的url
 *
 *  @param url     接口路径，如/path/getArticleList
 *  @param params  接口中所需的参数，如@{"categoryid" : @(12)}
 *  @param success 接口成功请求到数据的回调
 *  @param fail    接口请求数据失败的回调
 *
 *  @return 返回的对象中有可取消请求的API
 */
+ (HTTPSessionTask *)postWithUrl:(NSString *)url
                      refreshCache:(BOOL)refreshCache
                            params:(NSDictionary *)params
                           success:(HTTPResponseSuccess)success
                              fail:(HTTPResponseFail)fail;
+ (HTTPSessionTask *)postWithUrl:(NSString *)url
                      refreshCache:(BOOL)refreshCache
                            params:(NSDictionary *)params
                          progress:(HYBPostProgress)progress
                           success:(HTTPResponseSuccess)success
                              fail:(HTTPResponseFail)fail;
/**
 *	@author Kael
 *
 *	图片上传接口，若不指定baseurl，可传完整的url
 *
 *	@param image			图片对象
 *	@param url				上传图片的接口路径，如/path/images/
 *	@param filename		给图片起一个名字，默认为当前日期时间,格式为"yyyyMMddHHmmss"，后缀为`jpg`
 *	@param name				与指定的图片相关联的名称，这是由后端写接口的人指定的，如imagefiles
 *	@param mimeType		默认为image/jpeg
 *	@param parameters	参数
 *	@param progress		上传进度
 *	@param success		上传成功回调
 *	@param fail				上传失败回调
 *
 *	@return
 */
+ (HTTPSessionTask *)uploadWithImage:(UIImage *)image
                                   url:(NSString *)url
                              filename:(NSString *)filename
                                  name:(NSString *)name
                              mimeType:(NSString *)mimeType
                            parameters:(NSDictionary *)parameters
                              progress:(HTTPUploadProgress)progress
                               success:(HTTPResponseSuccess)success
                                  fail:(HTTPResponseFail)fail;

/**
 *	@author Kael
 *
 *	上传文件操作
 *
 *	@param url						上传路径
 *	@param uploadingFile	待上传文件的路径
 *	@param progress			上传进度
 *	@param success				上传成功回调
 *	@param fail					上传失败回调
 *
 *	@return
 */
+ (HTTPSessionTask *)uploadFileWithUrl:(NSString *)url
                           uploadingFile:(NSString *)uploadingFile
                                progress:(HTTPUploadProgress)progress
                                 success:(HTTPResponseSuccess)success
                                    fail:(HTTPResponseFail)fail;


/*!
 *  @author Kael
 *
 *  下载文件
 *
 *  @param url           下载URL
 *  @param saveToPath    下载到哪个路径下
 *  @param progressBlock 下载进度
 *  @param success       下载成功后的回调
 *  @param failure       下载失败后的回调
 */
+ (HTTPSessionTask *)downloadWithUrl:(NSString *)url
                            saveToPath:(NSString *)saveToPath
                              progress:(HTTPDownloadProgress)progressBlock
                               success:(HTTPResponseSuccess)success
                               failure:(HTTPResponseFail)failure;
























@end
