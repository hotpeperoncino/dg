#ifndef _DG_DFS_H_
#define _DG_DFS_H_

#include "Analysis.h"
#include "NodesWalk.h"
#include "ADT/Queue.h"

using dg::ADT::QueueLIFO;

namespace dg {
namespace analysis {

enum DFSFlags {
    DFS_INTERPROCEDURAL         = 1 << 0,
    DFS_PARAMS                  = 1 << 1,
    DFS_CD                      = 1 << 2,
    DFS_DD                      = 1 << 3,
    DFS_REV_CD                  = 1 << 4,
    DFS_REV_DD                  = 1 << 5,
    // go through CFG edges between
    // basic blocks (enqueue first
    // nodes of BB successors for _every_ node)
    DFS_BB_CFG                  = 1 << 6,
    DFS_BB_REV_CFG              = 1 << 7,
    DFS_BB_POSTDOM              = 1 << 8,
    DFS_BB_POSTDOM_FRONTIERS    = 1 << 9,

    DFS_BB_NO_CALLSITES         = 1 << 10,
};


static inline
uint32_t convertFlags(uint32_t opts)
{
    uint32_t ret = 0;

    if (opts & DFS_INTERPROCEDURAL)
        ret |= NODES_WALK_INTERPROCEDURAL;
    if (opts & DFS_CD)
        ret |= NODES_WALK_CD;
    if (opts & DFS_DD)
        ret |= NODES_WALK_DD;
    if (opts & DFS_REV_CD)
        ret |= NODES_WALK_REV_CD;
    if (opts & DFS_REV_DD)
        ret |= NODES_WALK_REV_DD;
    if (opts & DFS_BB_CFG)
        ret |= NODES_WALK_BB_CFG;
    if (opts & DFS_BB_REV_CFG)
        ret |= NODES_WALK_BB_REV_CFG;
    if (opts & DFS_BB_POSTDOM)
        ret |= NODES_WALK_BB_POSTDOM;
    if (opts & DFS_BB_POSTDOM_FRONTIERS)
        ret |= NODES_WALK_BB_POSTDOM_FRONTIERS;

    assert(!(opts & DFS_PARAMS) && "Not implemented yet");
    assert(!(opts & DFS_INTERPROCEDURAL) && "Not implemented yet");
    assert(!(opts & DFS_BB_NO_CALLSITES) && "Not implemented yet");
    assert(!(opts & DFS_BB_POSTDOM) && "Not implemented yet");

    return ret;
}

template <typename NodeT>
class DFS : public NodesWalk<NodeT, QueueLIFO<NodeT *>>
{
public:
    DFS<NodeT>(uint32_t opts)
        : NodesWalk<NodeT, QueueLIFO<NodeT *>>(convertFlags(opts)),
          dfsorder(0), flags(opts) {}

    template <typename FuncT, typename DataT>
    void run(NodeT *entry, FuncT func, DataT data)
    {
        this->walk(entry, func, data);
    }

    template <typename FuncT, typename DataT>
    void operator()(NodeT *entry, FuncT func, DataT data)
    {
        run(entry, func, data);
    }

protected:
    /* virtual */
    void prepare(NodeT *BB)
    {
        // set dfs order number
        AnalysesAuxiliaryData& aad = this->getAnalysisData(BB);
        aad.dfsorder = ++dfsorder;
    }
private:
    unsigned int dfsorder;
    uint32_t flags;
};

#ifdef ENABLE_CFG

static uint32_t inline
convertBBFlags(uint32_t flags)
{
    uint32_t ret = 0; // for BBs we always have CFG

    if (flags & DFS_INTERPROCEDURAL)
        ret |= BBLOCK_WALK_INTERPROCEDURAL;
    if (flags & DFS_PARAMS)
        ret |= BBLOCK_WALK_PARAMS;
    if (flags & DFS_BB_NO_CALLSITES)
        ret |= BBLOCK_NO_CALLSITES;
    if (flags & DFS_BB_CFG)
        ret |= BBLOCK_WALK_CFG;

    return ret;
}

template <typename NodeT>
class BBlockDFS : public BBlockWalk<NodeT,
                                    QueueLIFO<BBlock<NodeT> *> >
{
public:
    typedef BBlock<NodeT> *BBlockPtrT;

    BBlockDFS<NodeT>(uint32_t fl = DFS_BB_CFG)
        : BBlockWalk<NodeT, QueueLIFO<BBlock<NodeT> *>>(convertBBFlags(fl)),
          dfsorder(0), flags(fl) {}

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
        // set dfs order number
        AnalysesAuxiliaryData& aad = this->getAnalysisData(BB);
        aad.dfsorder = ++dfsorder;
    }
private:
    unsigned int dfsorder;
    uint32_t flags;
};
#endif // ENABLE_CFG

} // namespace analysis
} // namespace dg

#endif // _DG_NODES_WALK_H_
