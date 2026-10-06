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

## ガチャ画面の復活 / Gacha ##

クライアントに残っているガチャ画面を、COMP ショップの一覧から開けるようにします。
CP で引き、景品は宅配に届きます。ショップ XML の `Type` を `GACHA` にしたときだけ動き、
対応したクライアント（非公開）が必要です。GM コマンド `@gacha` やタブの条件で、表示・非表示と
景品の切り替えができます。

詳しくは [GACHA.md](GACHA.md) を見てください。

*English: Restores the client's gacha window. Shops with `Type` `GACHA` are
listed in the COMP shop menu (needs a client that supports it, not public); drawing charges CP and
sends a weighted random prize to the post. Toggle with `@gacha` or tab
conditions. See GACHA.md.*
## デモンフォースのまとめ使い / Bulk demon force ##

悪魔にフォース用アイテムを使うとき、1回の使用で同じアイテムを複数個まとめて
消費します（クライアントの変更は不要です）。

### フォースの仕組み（元の動作） ###

- アイテム1個で、20種類の能力値フォース（HP最大・力・経験値など）に少しずつ
  ポイントが入ります。100,000 ポイントで能力値 +1 です（`DEMON_FORCE_PRECISION`）。
  上限はアイテムごとに `DevilBoostData` の結果欄で決まります。
- アイテム1個でベネフィットゲージが +1 されます。ゲージが 10・30・60、その後は
  100 ごと（`DevilBoostLotData`）になると、パッシブ（`DevilBoostExtraData` の特性）
  の候補が抽選で1つ出ます。候補を枠（最大8枠）に入れるか捨てるまで、次のアイテムは
  使えません。
- 特定のパッシブを直接枠に入れるアイテムもあります。

### まとめ使いの動作 ###

- 1回の使用で最大 N 個を続けて使います。次の場合はその時点で止まります。
  - パッシブの候補が出たとき（いつもの選択画面が出ます）
  - 能力値フォースがそのアイテムの上限に達したとき
  - 持っているアイテムが無くなったとき（同じアイテムの別の山からも使います）
- 枠に直接入れるアイテムは、今までどおり1個ずつです。
- ゲージだけを上げるアイテム（「契の玉・回」など）もまとめて使われ、パッシブ候補が出るか、
  アイテムが無くなるまで続きます。

### `@force` コマンド ###

有効なときは一般プレイヤーも使えます。

- `@force`: 召喚中の悪魔のゲージ、次のパッシブ候補までの残り個数、各フォースの値を表示
- `@force 個数`: 1回で使う個数を設定（1〜上限。ログアウトすると上限に戻ります）
- `@force max`: 上限に戻す
- `@force effect off` / `on`: ゲージ満タン時の悪魔のエフェクトを出さない / 出す（ログアウトで「出す」に戻ります）。
  画面の画像表示と約4秒の待ち時間はクライアント側の演出なので変わりません

### 設定 ###

`config/constants.xml` に次の行を追加すると有効になります。数字は1回で使う上限です。

```xml
<constant name="DEMON_FORCE_BULK">100</constant>
```

`0`・`1` または行が無い場合は無効です（1個ずつ、`@force` は GM のみ）。

### 関係するソース ###

- `libhack/src/ServerConstants.h/.cpp`: `DEMON_FORCE_BULK`（省略可能）
- `server/channel/schema/clientstate.xml`: `DemonForceBulk`（`@force` で決めた個数、保存しない）
- `server/channel/src/packets/game/DemonForce.cpp`: まとめ使いの処理
- `server/channel/src/ChatManager.h/.cpp`: `@force`

*English: With `DEMON_FORCE_BULK` = N in constants.xml, one demon force use
consumes up to N of the same item, stopping when a new force stack effect
becomes pending, when nothing would be raised, or when the items run out.
`@force` (open to players while enabled) shows the gauge, uses left until the
next stack effect and the force values, and sets the per-use count.*
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
- チャット欄には何も出しません。今のランクや上げられる数は、スクリプトがキャラクターの
  ゾーンフラグに入れ、イベントの会話（`EventExNPCMessage` の `%d`）で表示します。
- 1 回の会話で上げるのは 1 回までにしてください（続けて上げるとクライアントが強制終了する
  ことがあります。上げた後は会話を終わらせる）。

### イベントから使う ###

- 条件スクリプト `bool_reunionAllRanks`（`EventScriptCondition`）
  - `value1`: 12 系統すべてがこのランク以上（8、99 など）
  - `value2`: 1 なら御霊化していることも条件にする
- アクションスクリプト `action_reunionBulk`（`ActionRunScript`）
  - `params`: 系統 1〜12（1 テイワズ … 12 ウィアド）、`info`（計算だけ）/ `ask` 数|`max`
    （上げる数を決める）/ `req`（決めた数だけ上げる）/ 数 / `max`
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

## プレイヤーだけのレベル上限 / Player-only level cap ##

プレイヤー（キャラクター）だけ、レベルの上限を 99 より上（最大 127）にできます。
悪魔の上限は今までどおり（ワールドの設定 `LevelCap`、標準 99）です。

- 上限は `constants.xml` で決めます。サーバーの再起動で変えられるので、
  「今は 110 まで、次の更新で 115 まで」のように少しずつ開放できます。
- 99 → 100 から先の必要な経験値も `constants.xml` に書きます（1 レベル 1 個、カンマ区切り）。
- 上限に届いたキャラクターは経験値が増えません（上限を上げると、そこから続きを貯められます）。
- 上限を下げても、すでに上限より上のキャラクターのレベルは下がりません（経験値が増えなくなるだけ）。
- 死んだときの経験値の減少は、上限の手前まで（今までは 99 未満）。
- アーツ（エキスパート）の上限ポイントは、レベル 100 以上でも 99 と同じ扱いです。
- GM コマンド `@levelup` も 127 まで指定できます（上限より上にはなりません）。
- イベントの条件 `LEVEL`（例: レベル 100 以上で入れる場所）は、そのまま 100 以上の値で使えます。
- クライアントのレベルは 1 バイト（符号付き）なので、127 より上にはできません。
  レベル 100 以上の表示は窓によって欄が足りないことがあります。

### 設定 ###

```xml
<!-- プレイヤーの上限（0 または行が無い = 悪魔と同じ上限） -->
<constant name="PLAYER_LEVEL_CAP">110</constant>
<!-- 99→100, 100→101, … の必要経験値（上限 - 99 個以上） -->
<constant name="PLAYER_LEVEL_XP">895842000000,990142000000,...</constant>
```

`PLAYER_LEVEL_CAP` が 127 を超える、または `PLAYER_LEVEL_XP` の数が足りない場合はサーバーが起動しません。

### 関係するソース ###

- `libhack/src/ServerConstants.h/.cpp`: `PLAYER_LEVEL_CAP`、`PLAYER_LEVEL_XP`（省略可能）
- `server/channel/src/CharacterManager.h/.cpp`: `GetLevelCap(character)`、`GetLevelXP(level)` と、
  経験値の加算・レベルアップ・死亡時の減少・アーツの上限
- `server/channel/src/EventManager.cpp`（悪魔クエストの経験値）、`ChatManager.cpp`（`@levelup`）

*English: `PLAYER_LEVEL_CAP` (up to 127) raises the level cap for player
characters only; demons keep the world level cap. `PLAYER_LEVEL_XP` lists the
experience needed for each level from 99 up. Both are read at startup, so the
cap can be raised step by step.*
