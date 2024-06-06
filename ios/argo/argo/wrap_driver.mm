#import "wrap_driver.h"
#include "driver.h"

@interface wrap_driver()
@property IDriver* driverItem;
@end

@implementation wrap_driver
- (instancetype)init
{
    if (self == [super init]) {
        self.driverItem = TheDriverPtr();
    }
    return self;
}
- (void)start
{
}
- (void)stop
{
}
@end

