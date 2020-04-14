This project uses Opencv 3.4
1. clone
2. Cmake

# Drawing epipolar lines
1. Epipolar/Input フォルダに　エピポーラ線を描きたい２つの画像を入れる
  - 左の画像にはepipolar_l.jpg 右の画像にはepiporar_r.jpgと名前をつけておく．
2. epipolarソリューションをbuild
  - buildフォルダのReleaseまたはDebugフォルダにepipolar.exeができる
3.　exe実行
  - 左右の対応点(初期設定8点)を同じ順序でクリックしていく．　
  - 因みにクリックされた箇所は表示されないので注意
  - Outputフォルダに左右それぞれの画像がクリックし終わると，クリック点が入った画像が出力される　確認用
  - 対応点の個数はマクロ定義のPNUMを変更する
4. 対応点を取り終わると，エピポーラ線が入った画像がOutputに出力される

# Mpsaic processing
1. Epipolar/Input フォルダに　mosaic処理をしたい２つの画像を入れる
  - 左の画像にはmosaic_l.jpg 右の画像にはmosaic_r.jpgと名前をつけておく．
2. mosaicソリューションをbuild
  - buildフォルダのReleaseまたはDebugフォルダにmosaic.exeができる
3.　exe実行
  - 左右の対応点(初期設定8点)を同じ順序でクリックしていく．　
  - 因みにクリックされた箇所は表示されないので注意
  - Outputフォルダに左右それぞれの画像がクリックし終わると，クリック点が入った画像が出力される　確認用
  - 対応点の個数はマクロ定義のPNUMを変更する
  - コンソールに対応点の座標が表示される　確認用
4. 対応点を取り終わると， 射影変換行列などがコンソールに表示される　確認用
  - クリック点が入った画像や，射影変換された画像，　combineされた画像等が出力される
!(https://github.com/ReinaIshikawa/EpipolarGeometry/blob/master/Epipolar/Output/combined.jpg?raw=true "combined image")
