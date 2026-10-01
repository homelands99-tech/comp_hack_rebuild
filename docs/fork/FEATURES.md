# 追加した機能 / Added features #

## 原料タンク自動収納 / Automatic material tank storage ##

敵のドロップや宝箱からアイテムを拾ったとき、原料タンクに収納できる素材を
自動で原料タンクに入れます。

- タンクに全部入る素材は、インベントリが満杯でも拾えます。
- タンクの上限を超えた分は、今までどおりインベントリに入ります。
- 収納したときは、チャット欄に「原料タンクに ○○ を N 個収納しました。（計 M）」と表示されます。
- 原料タンク（貴重品 `VALUABLE_MATERIAL_TANK`）を持っていないキャラクターでは何も変わりません。
- 収納できるかどうかの判定は、手動でタンクに入れるとき（`MaterialInsert`）と同じです
  （`TankData.sbin` と `DisassemblyTriggerData.sbin` に登録された素材）。
- 対象は「拾ったとき」だけです。クエスト報酬・ショップ購入・トレードなどは対象外です。
- データベースの構造は変更しません。

### 設定 ###

`config/constants.xml` に次の行を追加すると有効になります。

```xml
<constant name="AUTO_MATERIAL_TANK">1</constant>
```

`0` または行が無い場合は無効です。

### 関係するソース ###

- `libhack/src/ServerConstants.h/.cpp`: `AUTO_MATERIAL_TANK`（省略可能）
- `libhack/src/DefinitionManager.h/.cpp`: `GetItemName()`（アイテム ID から名前）
- `server/channel/src/CharacterManager.h/.cpp`: `GetMaterialTankSpace()`、`StoreInMaterialTank()`
- `server/channel/src/packets/game/LootItem.cpp`: アイテムを拾う処理への組み込み

*English: When a character with the material tank loots items, materials that
can be stored in the tank go there automatically (up to the tank limit; the
rest goes to the inventory). Loot that fits in the tank can be picked up even
with a full inventory. Enable with `AUTO_MATERIAL_TANK` = 1 in
constants.xml.*

## 一括転生（イベント用） / Bulk reunion for events ##

御霊化した仲魔の転生ランク（テイワズ〜ウィアドの 12 系統）を、NPC のイベントから
まとめて上げるための仕組みです。会話や選択肢はイベントの XML で自由に作れます。

### 動作 ###

- 対象: 召喚中の仲魔が御霊化していて、12 系統すべてがランク 8 以上のとき。
- 1 ランクごとの費用（まとめて支払い、足りなければ何も変わりません）:
  - マッカ `REUNION_BULK_COST`
  - レベルダウン防止のアイテム 1 個（`REUNION_BULK_KEEP_ITEMS`、初期値はイビルガム【100】の
    21894（譲渡不可）・21907。どちらでも、混ざっていても可。書いた順に使う＝譲渡不可から）
  - その系統のランク 9 の転生素材 1 回分（`DevilLVUpRateData` の 4 種類のどれか）。
    必要個数の多い素材から使い、全系統共通の素材 `REUNION_BULK_LAST_ITEMS`
    （初期値は原初のルーンストーン 21590）は最後に使います。
- レベルは下がらず、成長タイプも変わりません。上限は world の `ReunionMax` です。
- 今のランク・上げられる数・費用・結果は、チャット欄に表示します。

### イベントから使う ###

- 条件スクリプト `bool_reunionAllRanks`（`EventScriptCondition`）
  - `value1`: 12 系統すべてがこのランク以上（8、99 など）
  - `value2`: 1 なら御霊化していることも条件にする
- アクションスクリプト `action_reunionBulk`（`ActionRunScript`）
  - `params`: 系統 1〜12（1 テイワズ … 12 ウィアド）、上げる数（数字 / `max` = 払えるところまで /
    `info` = 表示だけ）
- スクリプトは datastore の `scripts` に置きます（このリポジトリには含めていません）。

### 設定 ###

```xml
<constant name="REUNION_BULK_COST">500000</constant>
```

`0` または行が無い場合は無効です。アイテムの一覧を変える場合は、
`REUNION_BULK_KEEP_ITEMS` / `REUNION_BULK_LAST_ITEMS` を他の一覧の定数と同じ形で書きます。

### 関係するソース ###

- `libhack/src/ServerConstants.h/.cpp`: `REUNION_BULK_COST`、`REUNION_BULK_KEEP_ITEMS`、`REUNION_BULK_LAST_ITEMS`（省略可能）
- `server/channel/src/CharacterManager.h/.cpp`: `ReunionBulk()`（スクリプトからも呼べる）

*English: `CharacterManager::ReunionBulk` (also bound for scripts) raises one
reunion group of the summoned mitama demon (all groups at rank 8+) by N ranks
at once, paying N x `REUNION_BULK_COST` macca, N level keeping items and N rank
9 material sets, without the level reset. Enable with `REUNION_BULK_COST`.*
