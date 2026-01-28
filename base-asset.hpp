#pragma once

class BaseAsset
{
public:
  BaseAsset() = default;
  BaseAsset(const BaseAsset &) = delete;
  virtual ~BaseAsset() = default;
};
