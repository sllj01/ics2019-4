#include <am.h>
#include <amdev.h>
#include <nemu.h>

//test
#include<klib.h>

#define W 400
#define H 300
#define FPS 30

//test
extern int screen_width();
extern int screen_height();
extern void draw_sync();

//static uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
static inline int min(int a, int b){
	return (a < b) ? a : b;
}

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      uint32_t tmp = inl(SCREEN_ADDR);
			info->width = (tmp & 0xffff0000) >> 16;
      info->height = tmp & 0xffff;
			//printf("SCREEN_REG=%d\n", inl(SCREEN_ADDR));
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
			
			int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
			//printf("x=%d, y=%d, w=%d, h=%d\n", x, y, w, h);
			uint32_t *pixels = ctl->pixels;
			//int cp_bytes = sizeof(uint32_t) * min(W, W-x);
			int cp_pixel_num = min(W, W-x);
			uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
			for(int j = 0; j < h && y + j < H; j++){
				//memcpy(&fb[(y+j)*W+x], pixels, cp_bytes);
				for(int k = 0; k < cp_pixel_num; k++){
					fb[(y+j)*W+x+k] = pixels[k];
					//fb[(y+j)*W+x+k] = 0xff00;
				}
				pixels += w;
			}
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }else{
			//	outl(SYNC_ADDR, 1);
			}
			//printf("sync=%d\n", ctl->sync);	
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
	int i;
	int size = screen_width() * screen_height();
	//printf("size=%d\n", size);
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	//printf("FB_ADDR=%x\n", FB_ADDR);
	for(i = 0; i < size; i++){
		fb[i] = 0xff00;
	}
	uint32_t tmp = inl(SCREEN_ADDR);
	uint32_t width = (tmp & 0xffff0000) >> 16;
  uint32_t height = tmp & 0xffff;
	printf("width=%d, height=%d\n", width, height);
	uint32_t tmp_sync = inl(SYNC_ADDR);
	printf("sync=%d\n", tmp_sync);	
	//outl(SYNC_ADDR, 1);
	draw_sync();

}
