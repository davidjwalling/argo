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
    self.driverItem->Title("Argo");
    self.driverItem->Copyright("Copyright 2010 David J. Walling. MIT License.");
    self.driverItem->Start(0, NULL);
}
- (void)stop
{
    self.driverItem->Stop();
}
@end

