#ifndef _DG_BFS_H_
#define _DG_BFS_H_

#include "Analysis.h"
#include "NodesWalk.h"
#include "ADT/Queue.h"

using dg::ADT::QueueFIFO;

namespace dg {
namespace analysis {

enum BFSFlags {
    BFS_INTERPROCEDURAL         = 1 << 0,
    BFS_PARAMS                  = 1 << 1,
    BFS_CFG                     = 1 << 2,
    BFS_REV_CFG                 = 1 << 3,
    BFS_CD                      = 1 << 4,
    BFS_DD                      = 1 << 5,
    BFS_REV_CD                  = 1 << 6,
    BFS_REV_DD                  = 1 << 7,

    BFS_BB_CFG                  = 1 << 8,
    BFS_BB_REV_CFG              = 1 << 9,
    BFS_BB_POSTDOM              = 1 << 10,
    BFS_BB_POSTDOM_FRONTIERS    = 1 << 11,

    BFS_BB_NO_CALLSITES         = 1 << 12,
};

#ifdef ENABLE_CFG

static inline uint32_t
convertBFSBBFlags(uint32_t flags)
{
    uint32_t ret = 0; // for BBs we always have CFG

    if (flags & BFS_INTERPROCEDURAL)
        ret |= BBLOCK_WALK_INTERPROCEDURAL;
    if (flags & BFS_BB_CFG)
        ret |= BBLOCK_WALK_CFG;
    if (flags & BFS_PARAMS)
        ret |= BBLOCK_WALK_PARAMS;
    if (flags & BFS_BB_POSTDOM)
        ret |= BBLOCK_WALK_POSTDOM;
    if (flags & BFS_BB_NO_CALLSITES)
        ret |= BBLOCK_NO_CALLSITES;

    return ret;
}

template <typename NodeT>
class BBlockBFS : public BBlockWalk<NodeT,
                                    QueueFIFO<BBlock<NodeT> *> >
{
public:
    typedef BBlock<NodeT> *BBlockPtrT;

    BBlockBFS<NodeT>(uint32_t fl = 0)
        : BBlockWalk<NodeT, QueueFIFO<BBlock<NodeT> *>>(convertBFSBBFlags(fl)),
          bfsorder(0), flags(fl) {}

    template <typename FuncT, typename DataT>
    void run(BBlockPtrT entry, FuncT func, DataT data)
    {
        this->walk(entry, func, data);
    }

    template <typename FuncT, typename DataT>
    void operator()(BBlockPtrT entry, FuncT func, DataT data)
    {
        run(entry, func, data);
    }

    uint32_t getFlags() const { return flags; }
protected:
    /* virtual */
    void prepare(BBlockPtrT BB)
    {
        // set bfs order number
        AnalysesAuxiliaryData& aad = this->getAnalysisData(BB);
        aad.bfsorder = ++bfsorder;
    }
private:
    unsigned int bfsorder;
    uint32_t flags;
};
#endif // ENABLE_CFG

} // namespace analysis
} // namespace dg

#endif // _DG_BFS_H_
