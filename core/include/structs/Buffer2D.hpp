#pragma once

#include "DoubleEnded.hpp"
#include "io/ColoredString.hpp"
#include "structs/HLine.hpp"
#include "utils/STL.hpp"

#include <ostream>
#include <string>

namespace details
{

/// <summary>
/// 2D buffer which expands as it's being written to, filling empty elements with neutral values.
/// </summary>
template <typename ContainerT>
class Buffer2D
{
public:
    class Line
    {
    private:
        DoubleEnded<ContainerT> line;

    public:
        using ValueT = ContainerT::value_type;
        using IndexT = DoubleEnded<ContainerT>::IndexT;

        static constexpr auto Npos = utils::npos<IndexT>;
        static const ValueT   WhiteSpace;

        Line()       = default;
        Line(Line&&) = default;
        Line(const IndexT begin, const IndexT end) noexcept;
        Line(const Line&) = default;
        Line(ContainerT&& str) noexcept;

        Line& operator=(Line&& other) = default;

        const DoubleEnded<ContainerT>& data() const;

        bool   empty() const;
        size_t size() const;

        IndexT beginIdx() const;
        IndexT endIdx() const;

        void expandTo(const IndexT size);
        void expandBy(const IndexT size);
        void expandWith(const ContainerT& str);
        void expandWith(const Line& other);

        void appendBack(const ValueT c);
        void appendFront(const ValueT c);
        void appendBack(const ContainerT& str);
        void appendFront(const ContainerT& str);
        void appendBack(const Line& other);
        void appendFront(const Line& other);

        void insert(const IndexT idx, const ContainerT& str);

        ContainerT extract(const IndexT begin, const IndexT end) const;

        IndexT findFirstNonWhiteSpace() const;
        IndexT findLastNonWhiteSpace() const;

        bool isWhiteSpace(const IndexT idx) const;
        bool isWhiteSpace() const;

        void clear();

        const ValueT operator[](const IndexT idx) const;
        ValueT&      operator[](const IndexT idx);

        template <typename U>
        friend std::ostream& operator<<(std::ostream& os, const Buffer2D<U>::Line& textLine);
    };

public:
    using ValueT = ContainerT::value_type;
    using IndexT = Line::IndexT;
    using PointT = Point<IndexT>;

private:
    DoubleEnded<std::vector<Line>> block;

    Buffer2D(const Buffer2D& other) noexcept;

    size_t getMaxPositiveWidth() const;
    size_t getMaxNegativeWidth() const;

    void expandTo(const IndexT idx);

public:
    static constexpr auto Npos = utils::npos<IndexT>;
    static const ValueT   WhiteSpace;

    Buffer2D()           = default;
    Buffer2D(Buffer2D&&) = default;
    Buffer2D(const ContainerT& str) noexcept;

    Buffer2D& operator=(Buffer2D&& other) = default;

    bool empty() const;
    void clear();

    IndexT beginIdx() const;
    IndexT endIdx() const;

    Buffer2D& appendRight(const Buffer2D& other, const ContainerT& padding = "");
    Buffer2D& appendRight(const ContainerT& str);

    void insert(const PointT& coords, const ContainerT& str);

    Buffer2D clone() const;

    bool isWhiteSpace(const PointT& coords) const;
    bool isWhiteSpace(const PointT& coords, const size_t width) const;
    bool isWhiteSpace(const HLine<IndexT>& section) const;

    Point<size_t> getTrimmedDimensions() const;

    PointT                    findFirstNonWhiteSpace() const;
    std::pair<IndexT, IndexT> getNonWhiteSpaceSpan(const IndexT idx) const;

    Line&       operator[](const IndexT idx);
    const Line& operator[](const IndexT idx) const;
    ValueT&     operator[](const PointT& coords);
    ValueT      operator[](const PointT& coords) const;
    ContainerT  operator[](const HLine<IndexT>& section) const;

private:
    template <typename StreamT>
    void dump(StreamT& os) const;

public:
    ContainerT toString() const;
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Buffer2D<U>& buffer);
};

template <typename ContainerT>
std::ostream& operator<<(std::ostream& os, const Buffer2D<ContainerT>& buffer)
{
    buffer.dump(os);
    return os;
}

}  // namespace details

using TextBlock        = details::Buffer2D<std::string>;
using ColoredTextBlock = details::Buffer2D<ColoredString>;
