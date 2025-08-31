#include "structs/Buffer2D.hpp"

#include <sstream>

//
// Line
//

template <typename ContainerT>
const typename Buffer2D<ContainerT>::Line::ValueT Buffer2D<ContainerT>::Line::WhiteSpace = ' ';

template <typename ContainerT>
Buffer2D<ContainerT>::Line::Line(const IndexT begin, const IndexT end) noexcept :
    line(begin, end, WhiteSpace)
{}

template <typename ContainerT>
Buffer2D<ContainerT>::Line::Line(ContainerT&& str) noexcept :
    line(std::move(str))
{}

template <typename ContainerT>
const DoubleEnded<ContainerT>& Buffer2D<ContainerT>::Line::data() const
{
    return line;
}

template <typename ContainerT>
bool Buffer2D<ContainerT>::Line::empty() const
{
    return line.empty();
}

template <typename ContainerT>
size_t Buffer2D<ContainerT>::Line::size() const
{
    return line.totalSize();
}

template <typename ContainerT>
Buffer2D<ContainerT>::Line::IndexT Buffer2D<ContainerT>::Line::beginIdx() const
{
    return line.beginIndex();
}

template <typename ContainerT>
Buffer2D<ContainerT>::Line::IndexT Buffer2D<ContainerT>::Line::endIdx() const
{
    return line.endIndex();
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::expandTo(const IndexT size)
{
    line.expandTo(size, WhiteSpace);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::expandBy(const IndexT size)
{
    line.expandBy(size, WhiteSpace);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::expandWith(const ContainerT& str)
{
    line.appendBack(str);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::expandWith(const Line& other)
{
    this->line.appendBack(other.line);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::appendBack(const ValueT c)
{
    line.pushBack(c);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::appendFront(const ValueT c)
{
    line.pushFront(c);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::appendBack(const ContainerT& str)
{
    line.appendBack(str);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::appendFront(const ContainerT& str)
{
    line.appendFront(str);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::appendBack(const Line& other)
{
    line.appendBack(other.line);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::appendFront(const Line& other)
{
    line.appendFront(other.line);
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::insert(const IndexT idx, const ContainerT& str)
{
    // TODO: could do in a single resize by separating positive from negative
    expandTo(idx);
    expandTo(idx + str.size());
    line.overwrite(idx, str);
}

template <typename ContainerT>
ContainerT Buffer2D<ContainerT>::Line::extract(const IndexT begin, const IndexT end) const
{
    return line.extract(begin, end);
}

template <typename ContainerT>
Buffer2D<ContainerT>::Line::IndexT Buffer2D<ContainerT>::Line::findFirstNonWhiteSpace() const
{
    if (const auto idx = line.backwardData().find_last_not_of(WhiteSpace); idx != ContainerT::npos)
        return -static_cast<IndexT>(idx) - 1;
    if (const auto idx = line.forwardData().find_first_not_of(WhiteSpace); idx != ContainerT::npos)
        return static_cast<IndexT>(idx);

    return Npos;
}

template <typename ContainerT>
Buffer2D<ContainerT>::Line::IndexT Buffer2D<ContainerT>::Line::findLastNonWhiteSpace() const
{
    if (const auto idx = line.forwardData().find_last_not_of(WhiteSpace); idx != ContainerT::npos)
        return static_cast<IndexT>(idx);
    if (const auto idx = line.backwardData().find_first_not_of(WhiteSpace); idx != ContainerT::npos)
        return -static_cast<IndexT>(idx) - 1;

    return Npos;
}

template <typename ContainerT>
bool Buffer2D<ContainerT>::Line::isWhiteSpace(const IndexT idx) const
{
    if (idx < line.beginIndex() || idx >= line.endIndex())
        return true;

    return line[idx] == WhiteSpace;
}

template <typename ContainerT>
bool Buffer2D<ContainerT>::Line::isWhiteSpace() const
{
    return line.backwardData().find_first_not_of(WhiteSpace) == ContainerT::npos &&
           line.forwardData().find_first_not_of(WhiteSpace) == ContainerT::npos;
}

template <typename ContainerT>
void Buffer2D<ContainerT>::Line::clear()
{
    line.clear();
}

template <typename ContainerT>
const typename Buffer2D<ContainerT>::Line::ValueT Buffer2D<ContainerT>::Line::operator[](const IndexT idx) const
{
    return line[idx];
}

template <typename ContainerT>
typename Buffer2D<ContainerT>::Line::ValueT& Buffer2D<ContainerT>::Line::operator[](const IndexT idx)
{
    expandTo(idx);
    return line[idx];
}

template <typename ContainerT>
std::ostream& operator<<(std::ostream& os, const typename Buffer2D<ContainerT>::Line& textLine)
{
    return os << textLine.line;
}

//
// Buffer2D
//

template <typename ContainerT>
const typename Buffer2D<ContainerT>::ValueT Buffer2D<ContainerT>::WhiteSpace = ' ';

template <typename ContainerT>
Buffer2D<ContainerT>::Buffer2D(const Buffer2D& other) noexcept :
    block(other.block)
{}

template <typename ContainerT>
Buffer2D<ContainerT>::Buffer2D(const ContainerT& str) noexcept
{
    if (str.empty())
        return;

    auto isWhiteSpaceLine   = true;
    auto foundFirstNonEmpty = false;
    auto lastNonEmpty       = utils::npos<size_t>;
    auto lastSep            = utils::npos<size_t>;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\n') {
            if (isWhiteSpaceLine) {
                if (foundFirstNonEmpty)
                    block.emplaceBack();  // Maintain distances on Y-axis.

                lastSep = i;
                continue;  // Skip initial empty lines.
            }

            auto line = str.substr(lastSep + 1, i - lastSep - 1);

            lastNonEmpty = block.forwardData().size();
            block.emplaceBack(std::move(line));
            lastSep          = i;
            isWhiteSpaceLine = true;
            continue;
        }

        if (str[i] != WhiteSpace)
            isWhiteSpaceLine = false;
    }

    if (not isWhiteSpaceLine) {
        block.emplaceBack(str.substr(lastSep + 1));
        return;
    }

    if (utils::isNPos(lastNonEmpty)) {
        // Empty block.
        block.clear();
        return;
    }

    for (auto i = lastNonEmpty + 1; i < block.forwardData().size(); ++i)
        block.popBack();  // Remove trailing empty lines.
}

template <typename ContainerT>
bool Buffer2D<ContainerT>::empty() const
{
    return block.empty();
}

template <typename ContainerT>
void Buffer2D<ContainerT>::clear()
{
    block.clear();
}

template <typename ContainerT>
Buffer2D<ContainerT>::IndexT Buffer2D<ContainerT>::beginIdx() const
{
    return block.beginIndex();
}

template <typename ContainerT>
Buffer2D<ContainerT>::IndexT Buffer2D<ContainerT>::endIdx() const
{
    return block.endIndex();
}

template <typename ContainerT>
size_t Buffer2D<ContainerT>::getMaxPositiveWidth() const
{
    size_t maxWidth = 0;
    for (const auto& line : block.backwardData())
        maxWidth = std::max(maxWidth, line.data().forwardData().size());
    for (const auto& line : block.forwardData())
        maxWidth = std::max(maxWidth, line.data().forwardData().size());

    return maxWidth;
}

template <typename ContainerT>
size_t Buffer2D<ContainerT>::getMaxNegativeWidth() const
{
    size_t maxWidth = 0;
    for (const auto& line : block.backwardData())
        maxWidth = std::max(maxWidth, line.data().backwardData().size());
    for (const auto& line : block.forwardData())
        maxWidth = std::max(maxWidth, line.data().backwardData().size());

    return maxWidth;
}

template <typename ContainerT>
void Buffer2D<ContainerT>::expandTo(const IndexT idx)
{
    block.expandTo(idx);
}

template <typename ContainerT>
Buffer2D<ContainerT>& Buffer2D<ContainerT>::appendRight(const Buffer2D& other, const ContainerT& padding)
{
    if (other.empty())
        return *this;

    if (this->empty()) {
        *this = other.clone();
        return *this;
    }

    auto       thisSize  = this->block.totalSize();
    const auto otherSize = other.block.totalSize();

    // Pad all lines to the largest positive width.
    const auto thisMaxPositiveWidth = this->getMaxPositiveWidth() - 1;
    for (auto& line : block.backwardData())
        line.expandTo(thisMaxPositiveWidth);
    for (auto& line : block.forwardData())
        line.expandTo(thisMaxPositiveWidth);

    // Expand and center using white-space lines to reach the size of the other block.
    if (thisSize < otherSize) {
        const auto diff = otherSize - thisSize;
        const auto half = diff / 2;

        size_t i = 0;
        for (; i < half; ++i)
            this->block.emplaceFront(0, thisMaxPositiveWidth);
        for (; i < diff; ++i)
            this->block.emplaceBack(0, thisMaxPositiveWidth);

        thisSize = otherSize;
    }

    // Append padding.
    if (padding.size()) {
        for (auto& line : this->block.backwardData())
            line.appendBack(padding);
        for (auto& line : this->block.forwardData())
            line.appendBack(padding);
    }

    // If the two blocks have the same size, each i-th index from this is mapped to
    // the i-th index from other. If the other block is smaller the first (diff / 2)
    // lines from this block are skipped, centering the other. This block cannot be
    // smaller because of the check above.
    const auto thisBegin  = this->block.beginIndex();
    const auto otherBegin = other.block.beginIndex();
    const auto otherEnd   = other.block.endIndex();

    const auto half     = (thisSize - otherSize) / 2;
    const auto idxShift = thisBegin - otherBegin + half;

    const auto otherNegativeWidth = other.getMaxNegativeWidth();

    for (Buffer2D<ContainerT>::IndexT i = otherBegin; i < otherEnd; ++i) {
        if (const auto linePadSize = otherNegativeWidth - other.block[i].data().backwardData().size())
            this->block[i + idxShift].appendBack(ContainerT(linePadSize, WhiteSpace));
        this->block[i + idxShift].appendBack(other.block[i]);
    }

    return *this;
}

template <typename ContainerT>
Buffer2D<ContainerT>& Buffer2D<ContainerT>::appendRight(const ContainerT& str)
{
    return appendRight(Buffer2D(str));
}

template <typename ContainerT>
void Buffer2D<ContainerT>::insert(const PointT& coords, const ContainerT& str)
{
    expandTo(coords.y);
    block[coords.y].insert(coords.x, str);
}

template <typename ContainerT>
Buffer2D<ContainerT> Buffer2D<ContainerT>::clone() const
{
    return *this;
}

template <typename ContainerT>
bool Buffer2D<ContainerT>::isWhiteSpace(const PointT& coords) const
{
    if (coords.y < block.beginIndex() || coords.y >= block.endIndex())
        return true;

    return block[coords.y].isWhiteSpace(coords.x);
}

template <typename ContainerT>
bool Buffer2D<ContainerT>::isWhiteSpace(const PointT& coords, const size_t width) const
{
    if (coords.y < block.beginIndex() || coords.y >= block.endIndex())
        return true;

    for (size_t i = 0; i < width; ++i)
        if (not block[coords.y].isWhiteSpace(coords.x + i))
            return false;

    return true;
}

template <typename ContainerT>
bool Buffer2D<ContainerT>::isWhiteSpace(const HLine<IndexT>& section) const
{
    return isWhiteSpace(section.origin, section.length);
}

template <typename ContainerT>
Point<size_t> Buffer2D<ContainerT>::getTrimmedDimensions() const
{
    const auto endIdx = block.endIndex();
    auto       idx    = block.beginIndex();

    while (idx < endIdx && block[idx].isWhiteSpace())
        ++idx;

    if (idx == endIdx)
        return Point<size_t>(0, 0);

    const auto topFirst   = idx;
    auto       bottomLast = idx;
    auto       leftFirst  = std::numeric_limits<IndexT>::max();
    auto       rightLast  = std::numeric_limits<IndexT>::lowest();

    while (idx < endIdx) {
        const auto& line = block[idx];

        const auto first = line.findFirstNonWhiteSpace();
        if (first == Line::Npos) {
            ++idx;
            continue;  // White space line.
        }
        const auto last = line.findLastNonWhiteSpace();

        bottomLast = idx;
        leftFirst  = std::min(leftFirst, first);
        rightLast  = std::max(rightLast, last);
        ++idx;
    }

    if (topFirst > bottomLast || leftFirst > rightLast)
        return Point<size_t>(0, 0);

    return Point<size_t>(rightLast - leftFirst + 1, bottomLast - topFirst + 1);
}

template <typename ContainerT>
Buffer2D<ContainerT>::PointT Buffer2D<ContainerT>::findFirstNonWhiteSpace() const
{
    for (auto y = block.beginIndex(); y < block.endIndex(); ++y)
        if (const auto x = block[y].findFirstNonWhiteSpace(); x != Line::Npos)
            return PointT(x, y);

    return utils::npos<PointT>;
}

template <typename ContainerT>
std::pair<typename Buffer2D<ContainerT>::IndexT, typename Buffer2D<ContainerT>::IndexT>
Buffer2D<ContainerT>::getNonWhiteSpaceSpan(const IndexT idx) const
{
    const auto begin = block[idx].findFirstNonWhiteSpace();
    if (begin == Line::Npos)
        return utils::npos<std::pair<IndexT, IndexT>>;

    const auto end = block[idx].findLastNonWhiteSpace();
    return std::make_pair(begin, end + 1);
}

template <typename ContainerT>
typename Buffer2D<ContainerT>::Line& Buffer2D<ContainerT>::operator[](const IndexT idx)
{
    expandTo(idx);
    return block[idx];
}

template <typename ContainerT>
const typename Buffer2D<ContainerT>::Line& Buffer2D<ContainerT>::operator[](const IndexT idx) const
{
    return block[idx];
}

template <typename ContainerT>
typename Buffer2D<ContainerT>::ValueT& Buffer2D<ContainerT>::operator[](const PointT& coords)
{
    return (*this)[coords.y][coords.x];
}

template <typename ContainerT>
Buffer2D<ContainerT>::ValueT Buffer2D<ContainerT>::operator[](const PointT& coords) const
{
    return (*this)[coords.y][coords.x];
}

template <typename ContainerT>
ContainerT Buffer2D<ContainerT>::operator[](const HLine<IndexT>& section) const
{
    return block[section.origin.y].extract(section.origin.x, section.origin.x + section.length);
}

template <typename ContainerT>
template <typename StreamT>
void Buffer2D<ContainerT>::dump(StreamT& os) const
{
    if (block.empty())
        return;

    // Compute the bounding box
    const auto endIdx = block.endIndex();
    auto       idx    = block.beginIndex();

    while (idx < endIdx && block[idx].isWhiteSpace())
        ++idx;

    if (idx == endIdx)
        return;  // Full white space.

    const auto topFirst   = idx;
    auto       bottomLast = idx;
    auto       leftFirst  = std::numeric_limits<IndexT>::max();

    while (idx < endIdx) {
        const auto& line = block[idx];

        const auto first = line.findFirstNonWhiteSpace();
        if (first == Line::Npos) {
            ++idx;
            continue;  // White space line.
        }

        bottomLast = idx;
        leftFirst  = std::min(leftFirst, first);

        ++idx;
    }

    // Dump the block
    const auto dumpLine = [&](const Buffer2D<ContainerT>::Line& line) {
        if (line.empty() || line.isWhiteSpace())
            return;

        // Trim excess white spaces from the right.
        const auto rightLast = line.findLastNonWhiteSpace();
        const auto endIdx    = std::min(rightLast + 1, line.data().endIndex());

        // Trim excess white spaces from the left, or pad with extra spaces if needed.
        const auto beginIdx = line.data().beginIndex();
        if (leftFirst < beginIdx) {
            const auto padSize = beginIdx - leftFirst;
            os << ContainerT(padSize, Buffer2D<ContainerT>::WhiteSpace);

            line.data().dump(os, beginIdx, endIdx);
            return;
        }

        line.data().dump(os, leftFirst, endIdx);
    };

    for (auto i = topFirst; i < bottomLast; ++i) {
        dumpLine(block[i]);
        os << '\n';
    }

    dumpLine(block[bottomLast]);
}

template <typename ContainerT>
ContainerT Buffer2D<ContainerT>::toString() const
{
    // A bit ugly but it allows a single dump() implementation to cover all cases.
    if constexpr (std::is_same_v<ContainerT, std::string>) {
        std::ostringstream os;
        dump(os);
        return os.str();
    }
    else {
        ContainerT os;
        dump(os);
        return os;
    }
}

//
// Definitions
//

template class Buffer2D<std::string>;
template class Buffer2D<ColoredString>;

template void Buffer2D<std::string>::dump<std::ostream>(std::ostream&) const;
template void Buffer2D<std::string>::dump<std::ostringstream>(std::ostringstream&) const;
template void Buffer2D<ColoredString>::dump<std::ostream>(std::ostream&) const;
template void Buffer2D<ColoredString>::dump<ColoredString>(ColoredString&) const;
