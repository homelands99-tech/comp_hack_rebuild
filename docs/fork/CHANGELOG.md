# 変更履歴 / Changelog #

新しい変更を上に追記します。

## 2026-10-06 ##

- ガチャ画面の復活（`GACHA_ENABLED`、既定は無効。説明は GACHA.md）を追加
- インベントリのページ切り替え `@bag`（`INVENTORY_PAGES`、既定は無効）を追加
- GM コマンド `@rpoint`（転生ポイントの表示・変更）を追加
- プレイヤーだけのレベル上限（`PLAYER_LEVEL_CAP`・`PLAYER_LEVEL_XP`、既定は無効）を追加
- 追加機能の文言を `data/custom_messages.xml` で変えられるようにした
- 修正: `@bag` で作ったページの箱が保存されず次回ログインできなくなることがあった
- 修正: デモンフォースのまとめ使いで、枠に直接入れた効果が次の候補から除外されなかった
- 修正: 一括転生で同じアイテムが支払い一覧に 2 回あると支払いが足りないまま上がった

## 2026-10-02 ##

- 一括転生（イベント用、`REUNION_BULK_COST`、既定は無効）を追加

## 2026-10-01 ##

- デモンフォースのまとめ使いと `@force` を追加（`DEMON_FORCE_BULK`、既定は無効）

## 2026-09-26 ##

- 公開用に整理（README、ビルド手順、再建の記録）
- サブモジュールの URL を絶対パスに変更
- `windows_build.bat` を追加

## 2026-09-25 ##

- 原料タンク自動収納を追加（`AUTO_MATERIAL_TANK`、既定は無効）
- `libcomp`・`sqrat`・依存ライブラリを同梱し、ソースからビルドできるように再建
  （詳細は [RECONSTRUCTION.md](RECONSTRUCTION.md)）

## ベース ##

- [HyperChiicken/SMT](https://github.com/HyperChiicken/SMT) `develop` のコミット `95b6ae26`（2022-04-08）
