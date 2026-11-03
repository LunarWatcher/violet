// This file exists just to make it slightly harder to include server files in a non-server build (and make it more
// obvious when something has been incorrectly included)
#ifndef VIOLET_ENABLE_SERVER
#error "Developer error: server file was included in non-server build"
#endif
