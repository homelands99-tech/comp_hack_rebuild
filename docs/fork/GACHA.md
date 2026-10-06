# ガチャ画面の復活 / Gacha #

クライアントに残っているガチャ画面を、COMP ショップのカテゴリ一覧から開けるように
します。CP を消費して抽選し、当たった景品は宅配（ポスト）に届きます。

- 使うには **サーバー（このブランチ）** と、**ガチャに対応したクライアント** の両方が必要です。
  クライアント側の変更は公開していません。
- ショップ XML の `Type` を `GACHA` にしたときだけ動きます。既存のショップには影響しません。
- データベースの構造は変更しません。

---

## 1. 仕組み ##

| 流れ | 内容 |
|---|---|
| 一覧に並ぶ | COMP ショップを開くと、`GACHA` のショップも一覧に並びます。サーバーはカテゴリの 1 バイト（今まで「Unknown」で 0 固定だったもの）を 1 にして送ります |
| 画面が開く | ガチャに対応したクライアントは、その値が 1 のカテゴリを「ガチャ」として扱い、ガチャ画面を開きます |
| 金額とカード | 「ガチャ」タブの最初の商品（**引く用の商品**）を見て、画面に「1回 ○CPで やる」とカードのアニメを出します |
| 引く | クライアントは引く用の商品を**購入数 0** で購入要求します |
| 抽選 | サーバーは CP を引き、景品タブの中から当たりやすさ（`GachaWeight`）で 1 つ選んで宅配に入れ、その商品番号を返します |
| 結果表示 | クライアントは返ってきた商品のアイテムを結果として表示します |

---

## 2. ガチャの作り方 ##

`datastore/shops/` に XML を 1 つ作ります（ファイル名は自由。例: `gacha-9901.xml`）。
サーバーの再起動で読み込まれます。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<objects>
  <object name="ServerShop">
    <member name="ShopID">9901</member>        <!-- 他のショップと重ならない番号 -->
    <member name="Name">テストガチャ</member>   <!-- COMP ショップの一覧に出る名前 -->
    <member name="Type">GACHA</member>
    <member name="Tabs">
      <!-- (1) 引く用の商品。タブ名は必ず「ガチャ」 -->
      <element>
        <object name="ServerShopTab">
          <member name="Name">ガチャ</member>
          <member name="Products">
            <element>
              <object name="ServerShopProduct">
                <member name="ProductID">55001</member>
                <member name="BasePrice">10</member>
              </object>
            </element>
          </member>
        </object>
      </element>
      <!-- (2) 景品。タブ名は自由、タブは何個でも -->
      <element>
        <object name="ServerShopTab">
          <member name="Name">景品</member>
          <member name="Products">
            <element>
              <object name="ServerShopProduct">
                <member name="ProductID">105</member>
                <member name="GachaWeight">70</member>
              </object>
            </element>
            <element>
              <object name="ServerShopProduct">
                <member name="ProductID">3</member>
                <member name="GachaWeight">5</member>
              </object>
            </element>
          </member>
        </object>
      </element>
    </member>
  </object>
</objects>
```

### 引く用の商品（「ガチャ」タブ） ###

- タブ名は **「ガチャ」** 固定です（クライアントがこの名前で探します）。最初の商品だけが使われます。
- **1 回の CP**: その商品の `ShopProductData` の **CPCost**。画面の「1回 ○CP」もこの値です。
  CPCost が 0 の商品のときだけ `BasePrice` を使います（その場合、画面の表示は 0CP のままです）。
- **カードの絵柄**: 商品の **アイテム番号** で決まります。画像
  `Interface/tga/gacha_card<アイテム番号><00〜15>.tga`（16 枚）が表示されます。

画像がそろっているアイテム（昔のガチャ券）と、それを使う商品の例:

| アイテム | 商品の例（CPCost） |
|---|---|
| 798 | 55001（10CP）、20927（10CP）、20124（15CP） |
| 797 | 20104（30CP） |
| 823 | 20906（45CP） |
| 825 | 60363（50CP） |
| 796 | 20118（60CP） |
| 794 | 60409（100CP） |
| 756 | 60256（300CP） |
| 793 | 60520（10CP） |
| 824 | 60354（20CP） |

オリジナルの絵柄にしたいときは、新しいアイテム（とそれを売る商品）を作り、
`gacha_card<新しいアイテム番号>00.tga` 〜 `15.tga` の 16 枚を用意します。

### 景品（「ガチャ」以外のタブ） ###

- `ProductID` の商品が 1 個（`ShopProductData` の stack 数）宅配に届きます。
- `GachaWeight` が当たりやすさです。省略すると 1、**0 にするとその景品は出ません**。
  確率は「自分の重み ÷ 開いているタブの重みの合計」です（例: 70 と 25 と 5 なら 70%・25%・5%）。
- 宅配の空きが無いと引けません（エラー表示になり、CP は減りません）。

---

## 3. 表示・非表示と中身の切り替え ##

| やりたいこと | 方法 | 再起動 |
|---|---|---|
| すぐに出す／隠す | GM コマンド `@gacha 9901 off` / `@gacha 9901 on` | 不要（再起動すると XML の設定に戻る） |
| 最初から隠しておく | XML に `<member name="Disabled">true</member>` | 必要 |
| 期間限定で出す | 「ガチャ」タブに期間の条件を付ける | 不要（時刻で自動） |
| 期間で中身を入れ替える | 景品タブごとに期間の条件を付ける | 不要（時刻で自動） |
| 中身を作り直す | XML を編集 | 必要 |

### GM コマンド `@gacha` ###

`@announce` と同じ権限レベル（`GM_CMD_LVL_ANNOUNCE`）が必要です。

```
@gacha              ガチャの一覧と状態（on/off、自分に見えているか）
@gacha 9901 off     9901 を隠す（一覧から消え、引けなくなる）
@gacha 9901 on      9901 を出す
```

- 変更は**サーバーを再起動するまで**有効です。
- 一覧はプレイヤーが COMP ショップを開き直したときに反映されます。
  開いたままの画面で引こうとしても、隠したガチャは引けません（エラー表示）。

### 期間の条件 ###

タブに `Conditions` を書くと、その条件を満たしている間だけ有効になります。

- **「ガチャ」タブ** に付けた場合: 条件外の間はガチャ自体が一覧に出ません。
- **景品タブ** に付けた場合: 条件外の間はそのタブの景品が抽選から外れます。

例: 10 月 1 日 0:00 〜 10 月 31 日 23:59（サーバーの PC の時計）

```xml
<object name="ServerShopTab">
  <member name="Name">10月の景品</member>
  <member name="Conditions">
    <element>
      <object name="EventCondition">
        <member name="type">TIMESPAN_DATETIME</member>
        <member name="value1">10010000</member>  <!-- 月日時分 MMddHHmm -->
        <member name="value2">10312359</member>
      </object>
    </element>
  </member>
  <member name="Products"> ... </member>
</object>
```

- `value1`〜`value2` の形式は **月日時分**（`MMddHHmm`）。年は指定できません（毎年くり返します）。
  12 月 → 1 月をまたぐ指定（例: `12200000`〜`01072359`）もできます。
- 毎日の時間帯だけにしたいとき: `TIMESPAN_WEEK` で `value1`=`70000`（7=毎日, 00:00）、
  `value2`=`72359` のように、曜日の桁に 7 を使います。
  ※ 曜日指定（0=日〜6=土）は元のサーバーの計算に不具合があり、正しく動かないので使わないでください。
- 他にもイベント用の条件（レベル、所持アイテム、クエスト状態など）が使えます。
  その場合、条件はプレイヤーごとに判定されます。

### 中身を入れ替えるときの考え方 ###

- 同じガチャで景品だけ入れ替える: 景品タブを期間ごとに分け、それぞれに期間の条件を付けます。
- 別のガチャとして出す: 別の `ShopID` の XML を作り、「ガチャ」タブの期間を分けます。
- どの期間にも当てはまらない景品タブしか無い状態で引くとエラーになります（CP は減りません）。
  期間の切れ目に注意してください。

---

## 4. クライアント ##

ガチャに対応していないクライアントでは、`GACHA` のショップは一覧に出ません
（今までどおりの COMP ショップはそのまま使えます）。クライアント側の変更は公開していません。

---

## 5. 関係するソース ##

| ファイル | 内容 |
|---|---|
| `libhack/schema/server_shop.xml` | `Type` に `GACHA`、`ServerShop.Disabled`、`ServerShopProduct.GachaWeight` を追加 |
| `libhack/src/ServerDataManager.cpp` | `GACHA` のショップも COMP ショップの一覧に入れる |
| `server/channel/src/Gacha.h` | 共通処理（「ガチャ」タブ、条件判定、表示できるか） |
| `server/channel/src/packets/game/CompShopList.cpp` | 表示できないガチャを一覧から外す／ガチャの印（1）を送る |
| `server/channel/src/packets/game/ShopBuy.cpp` | 抽選・CP 消費・宅配（`HandleGachaDraw`）、購入数 0 でも処理 |
| `server/channel/src/packets/game/ShopData.cpp` | ガチャは相場変動なし |
| `server/channel/src/ChatManager.cpp` | GM コマンド `@gacha` |

## 6. 通信の決まり ##

- ガチャ画面を開くと、クライアントはショップデータ（`PACKET_SHOP_DATA`）と CP 残高
  （`PACKET_CASH_BALANCE`）を要求します。
- 引くと、「ガチャ」タブの最初の商品を **購入数 0** で購入要求（`PACKET_SHOP_BUY`）します。
- 返信（`PACKET_SHOP_BUY`）: 商品番号 = 表示する景品、結果 < 0 = エラー。最後の s32 が 0 以外だと
  別の結果メッセージになります（今は 0 固定）。
