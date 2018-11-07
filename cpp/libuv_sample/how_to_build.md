# How to build
buildに手こずったのでメモ。

## build libuv
基本は公式ドキュメントを参考にすれば良い  
https://github.com/nikhilm/uvbook/tree/master/libuv
```
$ https://github.com/nikhilm/uvbook.git

$ cd uvbook/libuv

// gypをインストール(clone)
$ git clone https://chromium.googlesource.com/external/gyp.git build/gyp

// outディレクトリができる
$ ./gyp_uv.py -f make                                                   

// ビルド。uvbook/libuv/out/Debug/libuv.aと静的ファイルができる(これが重要)
$ make -C out
CC(target) /home/vagrant/workspace/uvbook/libuv/out/Debug/obj.target/libuv/src/fs-poll.o
CC(target) /home/vagrant/workspace/uvbook/libuv/out/Debug/obj.target/libuv/src/inet.o
...

```

## build program linked with libuv
gccの使い方がなってなかっただけでもあるがこっちが苦労したのでメモ。  
例えば公式にある以下のファイルをビルドしたい場合。
```
// main.c

#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

int main() {
    uv_loop_t *loop = malloc(sizeof(uv_loop_t));
    uv_loop_init(loop);

    printf("Now quitting.\n");
    uv_run(loop, UV_RUN_DEFAULT);

    uv_loop_close(loop);
    free(loop);
    return 0;
}
```

```
// -lpthreadが必要(Centos7にはデフォルトで入っていた)
$ gcc -Iuvbook/libuv/include -Luvbook/libuv/out/Debug main.c -luv -lpthread main
```
