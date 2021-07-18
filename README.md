# Uty3D

## lod2opt

wavefont obj ファイル内で複数の material 定義を１つにマージします。
画像ファイルも１つにまとめ、解像度は
- 512 x 512
- 1024 x 1024
- 2048 x 2048
- 4096 x 4096
- 8192 x 8192
- 16384 x 16384
のいずれかになります。

国土交通省のPLATEAU [プラトー]で公開されている .obj 形式の3Dデータを他のツールで扱いやすくするするため作成しました。
そのため PLATEAU で公開されている .obj を想定していますので .obj ファイルを変換できるかどうかはわかりません。

## ビルド

Windows での VS2017 ソリューション、C++プロジェクトです。

### 依存ライブラリ

opencv 4.3 を使用しています。
プロジェクト内で環境変数を利用して include path と　リンクライブラの path を指定します。

OPENCV?DIR   opencv include フォルダ
OPENCV?LIB   opencv static library フォルダ (x64\vc15\staticlib)

## 使用方法

(1) ファイル指定
```
lod2opt.exe <hoge.obj> <dolder>
```

指定した .obj　ファイル内の material を１つにまとめ<folder> に .obj .mtl .jpg を作成します。

(2) フォルダ指定
```
lod2opt.exe <target-folder> <dolder>
```

指定フォルダ以下を再帰的に検索し見つかった .obj ファイルを全て変換します。


## ライセンス

MITライセンス

