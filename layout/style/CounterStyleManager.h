/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef mozilla_CounterStyleManager_h_
#define mozilla_CounterStyleManager_h_

#include "nsStringFwd.h"
#include "nsRefPtrHashtable.h"
#include "nsHashKeys.h"

#include "nsStyleConsts.h"

#include "mozilla/Attributes.h"

#include "nsCSSValue.h"

class nsPresContext;

namespace mozilla {

class WritingMode;

typedef int32_t CounterValue;

class CounterStyleManager;
class AnonymousCounterStyle;

struct NegativeType;
struct PadType;

class CounterStyle
{
protected:
  explicit constexpr CounterStyle(int32_t aStyle)
    : mStyle(aStyle)
  {
  }

private:
  CounterStyle(const CounterStyle& aOther) = delete;
  void operator=(const CounterStyle& other) = delete;

public:
  int32_t GetStyle() const { return mStyle; }
  bool IsNone() const { return mStyle == NS_STYLE_LIST_STYLE_NONE; }
  bool IsCustomStyle() const { return mStyle == NS_STYLE_LIST_STYLE_CUSTOM; }
  // A style is dependent if it depends on the counter style manager.
  // Custom styles are certainly dependent. In addition, some builtin
  // styles are dependent for fallback.
  bool IsDependentStyle() const;

  virtual void GetStyleName(nsSubstring& aResult) = 0;
  virtual void GetPrefix(nsSubstring& aResult) = 0;
  virtual void GetSuffix(nsSubstring& aResult) = 0;
  void GetCounterText(CounterValue aOrdinal,
                      WritingMode aWritingMode,
                      nsSubstring& aResult,
                      bool& aIsRTL);
  virtual void GetSpokenCounterText(CounterValue aOrdinal,
                                    WritingMode aWritingMode,
                                    nsSubstring& aResult,
                                    bool& aIsBullet);

  // XXX This method could be removed once ::-moz-list-bullet and
  //     ::-moz-list-number are completely merged into ::marker.
  virtual bool IsBullet() = 0;

  virtual void GetNegative(NegativeType& aResult) = 0;
  /**
   * This method returns whether an ordinal is in the range of this
   * counter style. Note that, it is possible that an ordinal in range
   * is rejected by the generating algorithm.
   */
  virtual bool IsOrdinalInRange(CounterValue aOrdinal) = 0;
  /**
   * This method returns whether an ordinal is in the default range of
   * this counter style. This is the effective range when no 'range'
   * descriptor is specified.
   */
  virtual bool IsOrdinalInAutoRange(CounterValue aOrdinal) = 0;
  virtual void GetPad(PadType& aResult) = 0;
  virtual CounterStyle* GetFallback() = 0;
  virtual uint8_t GetSpeakAs() = 0;
  virtual bool UseNegativeSign() = 0;

  virtual void CallFallbackStyle(CounterValue aOrdinal,
                                 WritingMode aWritingMode,
                                 nsSubstring& aResult,
                                 bool& aIsRTL);
  virtual bool GetInitialCounterText(CounterValue aOrdinal,
                                     WritingMode aWritingMode,
                                     nsSubstring& aResult,
                                     bool& aIsRTL) = 0;

  virtual AnonymousCounterStyle* AsAnonymous() { return nullptr; }

  NS_INLINE_DECL_PURE_VIRTUAL_REFCOUNTING

protected:
  int32_t mStyle;
};

class AnonymousCounterStyle final : public CounterStyle
{
public:
  explicit AnonymousCounterStyle(const nsSubstring& aContent);
  explicit AnonymousCounterStyle(const nsCSSValue::Array* aValue);

  virtual void GetStyleName(nsAString& aResult) override;
  virtual void GetPrefix(nsAString& aResult) override;
  virtual void GetSuffix(nsAString& aResult) override;
  virtual bool IsBullet() override;

  virtual void GetNegative(NegativeType& aResult) override;
  virtual bool IsOrdinalInRange(CounterValue aOrdinal) override;
  virtual bool IsOrdinalInAutoRange(CounterValue aOrdinal) override;
  virtual void GetPad(PadType& aResult) override;
  virtual CounterStyle* GetFallback() override;
  virtual uint8_t GetSpeakAs() override;
  virtual bool UseNegativeSign() override;

  virtual bool GetInitialCounterText(CounterValue aOrdinal,
                                     WritingMode aWritingMode,
                                     nsSubstring& aResult,
                                     bool& aIsRTL) override;

  virtual AnonymousCounterStyle* AsAnonymous() override { return this; }

  bool IsSingleString() const { return mSingleString; }
  uint8_t GetSystem() const { return mSystem; }
  const nsTArray<nsString>& GetSymbols() const { return mSymbols; }

  NS_INLINE_DECL_REFCOUNTING(AnonymousCounterStyle, override)

private:
  ~AnonymousCounterStyle() {}

  bool mSingleString;
  uint8_t mSystem;
  nsTArray<nsString> mSymbols;
};

class CounterStyleManager final
{
private:
  ~CounterStyleManager();
public:
  explicit CounterStyleManager(nsPresContext* aPresContext);

  static void InitializeBuiltinCounterStyles();

  void Disconnect();

  bool IsInitial() const
  {
    // only 'none' and 'decimal'
    return mCacheTable.Count() == 2;
  }

  CounterStyle* BuildCounterStyle(nsIAtom* aName);

  static CounterStyle* GetBuiltinStyle(int32_t aStyle);
  static CounterStyle* GetNoneStyle()
  {
    return GetBuiltinStyle(NS_STYLE_LIST_STYLE_NONE);
  }
  static CounterStyle* GetDecimalStyle()
  {
    return GetBuiltinStyle(NS_STYLE_LIST_STYLE_DECIMAL);
  }

  // This method will scan all existing counter styles generated by this
  // manager, and remove or mark data dirty accordingly. It returns true
  // if any counter style is changed, false elsewise. This method should
  // be called when any counter style may be affected.
  bool NotifyRuleChanged();

  nsPresContext* PresContext() const { return mPresContext; }

  NS_INLINE_DECL_REFCOUNTING(CounterStyleManager)

private:
  nsPresContext* mPresContext;
  nsRefPtrHashtable<nsRefPtrHashKey<nsIAtom>, CounterStyle> mCacheTable;
};

} // namespace mozilla

#endif /* !defined(mozilla_CounterStyleManager_h_) */
