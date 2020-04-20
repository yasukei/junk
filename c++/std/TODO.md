# Reference
https://ja.cppreference.com/w/cpp/header

# コンセプトライブラリ
<concepts> (C++20以上)	基礎的なライブラリコンセプト

# コルーチンライブラリ
<coroutine> (C++20以上)	コルーチンサポートライブラリ

# ユーティリティライブラリ
<cstdlib>	汎用ユーティリティ: プログラム制御、動的メモリ確保、乱数、ソートと検索
<csignal>	シグナル管理のための関数とマクロ定数
<csetjmp>	実行コンテキストを保存 (およびジャンプ) するマクロ (および関数)
<cstdarg>	可変長引数リストの処理
<typeinfo>	実行時型情報ユーティリティ
<typeindex> (C++11以上)	std::type_index
<type_traits> (C++11以上)	コンパイル時型情報
<bitset>	std::bitset クラステンプレート
<functional>	関数オブジェクト、関数呼び出し、バインド操作、参照ラッパー
<utility>	様々なユーティリティ部品
<ctime>	C スタイルの日付と時間のユーティリティ
<chrono> (C++11以上)	C++ の時間ユーティリティ
<cstddef>	標準的なマクロと typedef
<initializer_list> (C++11以上)	std::initializer_list クラステンプレート
<tuple> (C++11以上)	std::tuple クラステンプレート
<any> (C++17以上)	std::any クラス
<optional> (C++17以上)	std::optional クラステンプレート
<variant> (C++17以上)	std::variant クラステンプレート
<compare> (C++20以上)	三方比較演算子サポート
<version> (C++20以上)	実装依存のライブラリ情報を供給します
<source_location> (C++20以上)	ソースコード位置情報を取得する手段を供給します

# 動的メモリ管理
<new>	低水準メモリ管理ユーティリティ
<memory>	高水準メモリ管理ユーティリティ
<scoped_allocator> (C++11以上)	ネストされたアロケータクラス
<memory_resource> (C++17以上)	多相アロケータとメモリリソース

# 数値の限界
<climits>	整数型の限界
<cfloat>	浮動小数点型の限界
<cstdint> (C++11以上)	固定サイズの型と他の型の限界
<cinttypes> (C++11以上)	書式マクロ、intmax_t および uintmax_t の数学と変換
<limits>	算術型の特性を問い合わせる標準化された方法

# エラー処理
<exception>	例外処理ユーティリティ
<stdexcept>	標準の例外オブジェクト
<cassert>	引数をゼロと比較する条件コンパイルされるマクロ
<system_error> (C++11以上)	プラットフォーム依存のエラーコード std::error_code の定義
<cerrno>	最後のエラー番号を保持するマクロ
# 文字列ライブラリ
<cctype>	文字の種類を判定する関数
<cwctype>	ワイド文字の種類を判定する関数
<cstring>	様々なナロー文字列処理関数
<cwchar>	様々なワイドおよびマルチバイト文字列処理関数
<cuchar> (C++11以上)	C スタイルの Unicode 文字変換関数
<string>	std::basic_string クラステンプレート
<string_view> (C++17以上)	std::basic_string_view クラステンプレート
<charconv> (C++17以上)	std::to_chars および std::from_chars
<format> (C++20以上)	書式化ライブラリ (std::format など)

# コンテナライブラリ
<array> (C++11以上)	std::array コンテナ
<vector>	std::vector コンテナ
<deque>	std::deque コンテナ
<list>	std::list コンテナ
<forward_list> (C++11以上)	std::forward_list コンテナ
<set>	std::set および std::multiset 連想コンテナ
<map>	std::map および std::multimap 連想コンテナ
<unordered_set> (C++11以上)	std::unordered_set および std::unordered_multiset 非順序連想コンテナ
<unordered_map> (C++11以上)	std::unordered_map および std::unordered_multimap 非順序連想コンテナ
<stack>	std::stack コンテナアダプタ
<queue>	std::queue および std::priority_queue コンテナアダプタ
<span> (C++20以上)	std::span ビュー

# イテレータライブラリ
<iterator>	範囲のイテレータ

# 範囲ライブラリ
<ranges> (C++20以上)	範囲アクセス、プリミティブ、要件、ユーティリティおよびアダプタ

# アルゴリズムライブラリ
<algorithm>	範囲に対する操作を行うアルゴリズム
<execution> (C++17以上)	並列版のアルゴリズムのための定義済みの実行ポリシー

# 数値演算ライブラリ
<cmath>	一般的な数学関数
<complex>	複素数型
<valarray>	値の配列を表現、操作するためのクラス
<random> (C++11以上)	乱数生成および分布
<numeric>	コンテナ内の値に対する数値演算
<ratio> (C++11以上)	コンパイル時有理数算術
<cfenv> (C++11以上)	浮動小数点環境にアクセスする関数
<bit> (C++20以上)	ビット操作関数
<numbers> (C++20以上)	数学定数

# 入出力ライブラリ
<iosfwd>	入出力ライブラリのすべてのクラスの前方宣言
<ios>	std::ios_base クラス、std::basic_ios クラステンプレートといくつかの typedef
<istream>	std::basic_istream クラステンプレートといくつかの typedef
<ostream>	std::basic_ostream, std::basic_iostream クラステンプレートといくつかの typedef
<iostream>	いくつかの標準ストリームオブジェクト
<fstream>	std::basic_fstream, std::basic_ifstream, std::basic_ofstream クラステンプレートといくつかの typedef
<sstream>	std::basic_stringstream, std::basic_istringstream, std::basic_ostringstream クラステンプレートといくつかの typedef
<syncstream> (C++20以上)	std::basic_osyncstream, std::basic_syncbuf と typedef
<strstream> (C++98で非推奨)	std::strstream, std::istrstream, std::ostrstream
<iomanip>	入出力の書式を制御するヘルパー関数
<streambuf>	std::basic_streambuf クラステンプレート
<cstdio>	C スタイルの入出力関数

# ローカライゼーションライブラリ
<locale>	ローカライゼーションユーティリティ
<clocale>	C のローカライゼーションユーティリティ
<codecvt> (C++11以上) (C++17で非推奨)	Unicode 変換機能

# 正規表現ライブラリ
<regex> (C++11以上)	正規表現処理をサポートするクラス、アルゴリズムおよびイテレータ

# アトミック操作ライブラリ
<atomic> (C++11以上)	アトミック操作ライブラリ

# スレッドサポートライブラリ
<thread> (C++11以上)	std::thread クラスとサポート関数
<stop_token> (C++20以上)	std::jthread のためのストップトークン
<mutex> (C++11以上)	相互排他プリミティブ
<shared_mutex> (C++14以上)	共有可能な相互排他プリミティブ
<future> (C++11以上)	非同期計算のためのプリミティブ
<condition_variable> (C++11以上)	スレッドの待機状態
<semaphore> (C++20以上)	セマフォ
<latch> (C++20以上)	ラッチ
<barrier> (C++20以上)	バリア

#ファイルシステムライブラリ
<filesystem> (C++17以上)	std::path クラスとサポート関数
