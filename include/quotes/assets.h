#pragma once

#include <map>
#include <optional>
#include <string>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <utility>

#include "quotes/asset.h"
#include "quotes/types.h"

namespace quotes {

class AssetsManager {
 private:
    friend class boost::serialization::access;

    template <class Archive>
    void save(Archive & archive, const unsigned int /*version*/) const {
        archive << static_cast<SizeType>(mAssets.size());

        for (auto & nameAsset : mAssets) {
            auto history = nameAsset.second.history();

            archive << nameAsset.second.name();
            archive << static_cast<SizeType>(history.size());

            for (auto & moment : history) {
                archive << moment.time;
                archive << moment.value;
            }
        }
    }

    template <class Archive>
    void load(Archive & archive, const unsigned int /*version*/) {
        SizeType assetsNumber;
        archive >> assetsNumber;

        for (SizeType a; a < assetsNumber; ++a) {
            std::string assetName;
            SizeType pointsNumber;
            archive >> assetName;
            archive >> pointsNumber;

            Asset asset{assetName};
            HistoryType history;
            for (SizeType p; p < pointsNumber; ++p) {
                HistoryPoint point;
                archive >> point.time;
                archive >> point.value;
                history.emplace_back(std::move(point));
            }

            asset.add(history);
            mAssets.emplace(std::make_pair(assetName, std::move(asset)));
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

 public:
    AssetsManager() = default;

    void addAsset(const std::string & name) noexcept;

    bool removeAsset(const std::string & name) noexcept;

    std::optional<Asset> getAsset(std::string name) noexcept;

 private:
    std::map<std::string, Asset> mAssets;
};

}  // namespace quotes
