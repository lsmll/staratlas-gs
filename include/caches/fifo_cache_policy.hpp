/**
 * \file
 * \brief FIFO cache policy implementation
 */
#ifndef FIFO_CACHE_POLICY_HPP
#define FIFO_CACHE_POLICY_HPP

#include "cache_policy.hpp"
#include <list>

namespace caches
{

/**
 * \brief FIFO (First in, first out) cache policy
 * \details FIFO policy in the case of replacement removes the first added element.
 *
 * That is, consider the following key adding sequence:
 * ```
 * A -> B -> C -> ...
 * ```
 * In the case a cache reaches its capacity, the FIFO replacement candidate policy
 * returns firstly added element `A`. To show that:
 * ```
 * # New key: X
 * Initial state: A -> B -> C -> ...
 * Replacement candidate: A
 * Final state: B -> C -> ... -> X -> ...
 * ```
 * An so on, the next candidate will be `B`, then `C`, etc.
 * \tparam Key Type of a key a policy works with
 */
template <typename Key>
class FIFOCachePolicy : public ICachePolicy<Key>
{
  public:
    FIFOCachePolicy() = default;
    ~FIFOCachePolicy() = default;

    void Insert(const Key &key) override
    {
        fifo_queue.emplace_front(key);
    }
    // handle request to the key-element in a cache
    void Touch(const Key &key) noexcept override
    {
        // nothing to do here in the FIFO strategy
    }
    // handle element deletion from a cache
    void Erase(const Key &key) noexcept override
    {
        fifo_queue.pop_back();
    }

    // return a key of a replacement candidate
    const Key &ReplCandidate() const noexcept override
    {
        return fifo_queue.back();
    }

  private:
    std::list<Key> fifo_queue;
};
} // namespace caches

#endif // FIFO_CACHE_POLICY_HPP
