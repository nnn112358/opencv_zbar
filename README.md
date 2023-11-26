# opencv_ZBar

OpenCV+Zbar Example for M5 UnitV2 Frameworks




## コンパイルに必要なライブラリのインストール

 コンパイルに必要なライブラリをインストールします
 
```
sudo apt install git cmake g++ libgtk2.0-dev 
sudo apt install libprotobuf-dev protobuf-compiler
```


## クロスコンパイラのインストール

まず、/opt/ディレクトリの下に「external」フォルダを作成して移動します。

```
$ sudo mkdir /opt/external
$ sudo chmod 777 /opt/external
$ cd /opt/external
```

### クロスコンパイラのダウンロード
Cortex-Aファミリ用GNUツールチェインをダウンロードします。M5StackがUnitV2のファームウェアのバージョンと、ツールチェインのバージョンを一致させる必要があります。
UnitV2に対応しているツールチェインは以下のファイルとなります。これをARM社のホームページからダウンロードします。
「gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf.tar.xz」

ここでは、curlでツールチェインをダウンロードします。curlがインストールされてなければ、ブラウザからアクセスしてファイルをダウンロードします。

```
$ curl -LO https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf.tar.xz
```

### クロスコンパイラのインストール

ダウンロードした圧縮ファイルを解凍します。

```
$ tar Jxfv gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf.tar.xz
```

### クロスコンパイラへのパスを通す
環境変数を編集して、binフォルダの下にあるクロスコンパイラの本体「

」、「arm-none-linux-gnueabihf-gcc」へのパスを通します。

```
$ gedit ~/.bashrc
```

.bashrcの末尾へ、以下のコマンドを追加します。異なるフォルダにインストールする場合にはご使用の環境に合わせて編集してください。

```.bashrc
export PATH="$PATH:/opt/external/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/"
```

以下のコマンドを実行して、コンパイラのバージョン情報が出れくれば、ツールチェインのインストールが完了です。
```
$ arm-none-linux-gnueabihf-g++ --version
arm-none-linux-gnueabihf-g++ (GNU Toolchain for the A-profile Architecture 10.2-2020.11 (arm-10.16)) 10.2.1 20201103
Copyright (C) 2020 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

### 注意事項
Ubuntuには、apt-getコマンドで簡単にインストールできる、g++-arm-linux-gnueabihf/gcc-arm-linux-gnueabihf　パッケージがあります。

Ubuntu22.04では、このコンパイラででビルドしたバイナリを生成すると、Unitv2で実行した時に以下の画面のようにバージョンミスマッチのエラーが発生します。

```
unitv2% ./bin_test
./test0: /lib/libc.so.6: version `GLIBC_2.33' not found (required by ./test0)
```
UnitV2のファームウェア内部のGLIBCなどのバージョンが違っているためです。
なお、Ubuntu20.04では、UnitV2のファームウェアとapt-getで入るコンパイラのバージョンが近いため、バイナリをUnitv2で実行することが可能です。Ubuntu20.04以外のOSを使う場合は、ここまでの作業が必要です。

## OpenCVのインストール
UnitV2には、画像処理ライブラリOpenCVがインストールされています。OpenCVのプログラムをビルドするために、OpenCVのインストールします。

### OpenCVのダウンロード
UnitV2 のファームウェアはOpenCV ver4.4.0 と OpenCV extra modules ver4.4.0がインストールされています。
これにあわせて、開発環境にも、OpenCV ver4.4.0をインストールします。
OpenCV ver4.4.0をgitからダウンロードします。

```
$ cd /opt/external/
$ git clone https://github.com/opencv/opencv.git -b 4.4.0 --depth 1 
$ git clone https://github.com/opencv/opencv_contrib.git -b 4.4.0 --depth 1 
```

### クロスコンパイラの設定

opencvのarm-gnueabiのビルド設定ファイルを編集します。

```
$ cd opencv
$ gedit ./platforms/linux/arm-gnueabi.toolchain.cmake 
```

C++コンパイラはarm-none-linux-gnueabihf-g++、Cのコンパイラはarm-none-linux-gnueabihf-gccを指定します。

```arm-gnueabi.toolchain.cmake 
set(GCC_COMPILER_VERSION "" CACHE STRING "GCC Compiler version")
set(GNU_MACHINE "arm-linux-gnueabi" CACHE STRING "GNU compiler triple")

#以下の2行を追加する。
set(CMAKE_C_COMPILER "arm-none-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "arm-none-linux-gnueabihf-g++")


include("${CMAKE_CURRENT_LIST_DIR}/arm.toolchain.cmake")
```

クロスコンパイラ(arm版)を使ってOpenCVをビルドした後に、
デバックを効率よく行うため、PC上でもOpenCVを使えるように、X64環境のビルドを行います。

### OpenCVのコンパイル

```
$ cd /opt/external/opencv/
$ cmake -B build/arm -DCMAKE_TOOLCHAIN_FILE=./platforms/linux/arm-gnueabi.toolchain.cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/ -DBUILD_LIST=tracking,imgcodecs,videoio,highgui,features2d,ml,xfeatures2d -DCMAKE_BUILD_TYPE=Release . 
$ cmake --build build/arm
$ cmake --install build/arm --prefix install/arm

$ cmake -B build/x64 -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/ -DBUILD_LIST=tracking,imgcodecs,videoio,highgui,features2d,ml,xfeatures2d -DCMAKE_BUILD_TYPE=Release . 
$ cmake --build build/x64
$ cmake --install build/x64 --prefix install/x64
```

## ncnnのインストール

ncnnは、Tencentが開発しているモバイルプラットフォーム向けに最適化された高性能なニューラルネットワーク推論コンピューティングフレームワークです。

### ncnnのダウンロード

ncnnはコンパイルするとバイナル内部に含まれるタイプのライブラリなので、UnitV2とのバージョンの制約はありません。この記事を執筆時点の最新バージョン"ver 20231027"をダウンロードします。

```
$ cd /opt/external/
$ git clone https://github.com/Tencent/ncnn.git -b 20231027 --depth 1 
```
### クロスコンパイラの設定

ncnnのarm-gnueabiのビルド設定ファイルを編集します。

```
$ cd /opt/external/ncnn
$ gedit ./toolchains/arm-linux-gnueabihf.toolchain.cmake
```

ビルド設定ファイルに、もともと書いてある、arm-linux-gnueabihf-gccとarm-linux-gnueabihf-g++の設定をコメントアウトし、arm-none-linux-gnueabihf-g++とarm-none-linux-gnueabihf-gccをコンパイラとして設定します。


```arm-linux-gnueabihf.toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

#以下のもともと書いてある2行をコメントアウトする。
#set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
#set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")

#以下の2行を追加する。
set(CMAKE_C_COMPILER "arm-none-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "arm-none-linux-gnueabihf-g++")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_FLAGS "-march=armv7-a -mfloat-abi=hard -mfpu=neon")
set(CMAKE_CXX_FLAGS "-march=armv7-a -mfloat-abi=hard -mfpu=neon")

# cache flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags")
```

NCNNのEXAMPLESでは、OpenCVライブラリを使っているため、
cmakeにOpenCVのパスを設定します。

```
$ cd /opt/external/ncnn
$ gedit ./examples/CMakeList.txt
```

テキストの先頭に次の行を追加します。

```examples/CMakeList.txt
set(OpenCV_DIR /opt/external/opencv/build/x64/)
```


クロスコンパイラ(arm版)を使ってncnnをビルドします。
その次に、デバックを効率よく行うため、PC上でもncnnを使えるように、X64環境のビルドを行います。

```
$ cmake -B build/arm -DCMAKE_TOOLCHAIN_FILE=./toolchains/arm-linux-gnueabihf.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DNCNN_VULKAN=OFF -DNCNN_BUILD_EXAMPLES=ON . 
$ cmake --build build/arm
$ cmake --install build/arm --prefix install/arm

$ cmake -B build/x64 -DNCNN_VULKAN=OFF -DCMAKE_BUILD_TYPE=Release -DNCNN_BUILD_EXAMPLES=ON . 
$ cmake --build build/x64  
$ cmake --install build/x64 --prefix install/x64
```


### モデル変換ツールへのパスを通す
環境変数を編集して、binフォルダの下にある「onnx2ncnn」などのツールへのパスを通します。

```
$ gedit ~/.bashrc
```

.bashrcの末尾へ、以下のコマンドを追加します。

``` ~/.bashrc
export PATH="$PATH:/opt/external/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/"
# 以下を追加
export PATH="$PATH:/opt/external/ncnn/install/x64/bin/"
```




## ZBARのインストール

ZBarは、さまざまなプラットフォームでバーコードを読み取るためのオープンソースのライブラリです。

```
$ cd /opt/external/
$ curl -LO https://jaist.dl.sourceforge.net/project/zbar/zbar/0.10/zbar-0.10.tar.bz2 
$ tar -jxvf zbar-0.10.tar.bz2 
```

ncnnと同じく、デバックを効率よく行うため、ARM環境のビルドとX64環境のビルドを行います。

```
$ cd /opt/external/zbar-0.10
$ env NM=nm CFLAGS="" ./configure --prefix=$(pwd)/build/arm --host=arm-none-linux-gnueabihf --build=x86_64-linux --enable-shared --without-gtk --without-python --without-qt --without-imagemagick --disable-video --without-xshm CC=arm-none-linux-gnueabihf-gcc CXX=arm-none-linux-gnueabihf-g++ 
$ make && make install

$ make clean \
    && env NM=nm CFLAGS="" ./configure --prefix=$(pwd)/build/x64 --enable-shared --without-gtk --without-python --without-qt --without-imagemagick --disable-video --without-xshm 
$ make && make install
```
