//
//  AppDelegate.m
//  url-redirct v2
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)getUrl:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
    NSString *urlString = [[event paramDescriptorForKeyword:keyDirectObject]stringValue];
    
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSDictionary *infoPlist = [mainBundle infoDictionary];
    
    NSString *targetNotificationIdentifier = [infoPlist objectForKey:@"TargetNotificationIdentifier"];
    
    if(targetNotificationIdentifier)
    {
        void *object;
        const void *keys[] =   {CFSTR("url")};
        const void *values[] = {(CFStringRef)urlString};
        CFDictionaryRef userInfo = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 1, NULL, NULL);
        CFStringRef notificationIdentifier = (CFStringRef)targetNotificationIdentifier;
        CFNotificationCenterRef distributedCenter = CFNotificationCenterGetDistributedCenter();
        CFNotificationCenterPostNotification(distributedCenter,
                                             notificationIdentifier,
                                             object,
                                             userInfo,
                                             true);
        CFRelease(userInfo);
    }
}

- (id)init
{
	if(!(self = [super init]))return self;
	
    NSAppleEventManager *em = [NSAppleEventManager sharedAppleEventManager];
    
    [em
     setEventHandler:self
     andSelector:@selector(getUrl:withReplyEvent:)
     forEventClass:kInternetEventClass
     andEventID:kAEGetURL];
    
    [em
     setEventHandler:self
     andSelector:@selector(getUrl:withReplyEvent:)
     forEventClass:'WWW!'
     andEventID:'OURL'];
		
	return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
     NSAppleEventManager *em = [NSAppleEventManager sharedAppleEventManager];
     [em removeEventHandlerForEventClass:kInternetEventClass andEventID:kAEGetURL];
     [em removeEventHandlerForEventClass:'WWW!' andEventID:'OURL'];
}

@end
