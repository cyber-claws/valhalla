#ifndef VALHALLA_THOR_DYNAMICCOST_H_
#define VALHALLA_THOR_DYNAMICCOST_H_

#include <valhalla/baldr/directededge.h>
#include <valhalla/baldr/nodeinfo.h>
#include <valhalla/loki/search.h>
#include <memory>

#include <valhalla/thor/hierarchylimits.h>

namespace valhalla {
namespace thor {

// TODO - should edge transition costs be separate method or part of
// the Get and Seconds methods?

/**
 * Base class for dynamic edge costing. This class is an abstract class
 * defining the interface for costing methods. Derived classes must implement
 * methods to consider if access is allowed, compute a cost to traverse an
 * edge, and a couple other methods required to setup A* heuristics and
 * sorting methods.
 */
class DynamicCost {
 public:
  DynamicCost(const boost::property_tree::ptree& pt);

  virtual ~DynamicCost();

  /**
   * Does the costing allow hierarchy transitions?
   * @return  Returns true if the costing model allows hierarchy transitions).
   */
  virtual bool AllowTransitions() const;

  /**
   * Checks if access is allowed for the provided directed edge.
   * This is generally based on mode of travel and the access modes
   * allowed on the edge. However, it can be extended to exclude access
   * based on other parameters.
   * @param edge      Pointer to a directed edge.
   * @param restriction Restriction mask. Identifies the edges at the end
   *                  node onto which turns are restricted at all times.
   *                  This mask is compared to the next edge's localedgeidx.
   * @param uturn     Is this a Uturn?
   * @param dist2dest Distance to the destination.
   * @return  Returns true if access is allowed, false if not.
   */
  virtual bool Allowed(const baldr::DirectedEdge* edge,
                       const uint32_t restriction, const bool uturn,
                       const float dist2dest) const = 0;

  /**
   * Checks if access is allowed for the provided node. Node access can
   * be restricted if bollards or gates are present. (TODO - others?)
   * @param  edge  Pointer to node information.
   * @return  Returns true if access is allowed, false if not.
   */
  virtual bool Allowed(const baldr::NodeInfo* node) const = 0;

  /**
   * Get the cost given a directed edge.
   * @param edge  Pointer to a directed edge.
   * @return  Returns the cost to traverse the edge.
   */
  virtual float Get(const baldr::DirectedEdge* edge) const = 0;

  /**
   * Returns the time (in seconds) to traverse the edge.
   * @param edge  Pointer to a directed edge.
   * @return  Returns the time in seconds to traverse the edge.
   */
  virtual float Seconds(const baldr::DirectedEdge* edge) const = 0;

  /**
   * Get the cost factor for A* heuristics. This factor is multiplied
   * with the distance to the destination to produce an estimate of the
   * minimum cost to the destination. The A* heuristic must underestimate the
   * cost to the destination. So a time based estimate based on speed should
   * assume the maximum speed is used to the destination such that the time
   * estimate is less than the least possible time along roads.
   */
  virtual float AStarCostFactor() const = 0;

  /**
   * Get the general unit size that can be considered as equal for sorting
   * purposes. The A* method uses an approximate bucket sort, and this value
   * is used to size the buckets used for sorting. For example, for time
   * based costs one might compute costs in seconds and consider any time
   * within 1.5 seconds of each other as being equal (for sorting purposes).
   * @return  Returns the unit size for sorting.
   */
  virtual float UnitSize() const = 0;

  /**
   * Set the distance from the destination where "not_thru" edges are allowed.
   * @param  d  Distance in meters.
   */
  void set_not_thru_distance(const float d);

  /**
   * Returns a function/functor to be used in location searching which will
   * exclude results from the search by looking at each edges attribution
   */
  virtual const loki::EdgeFilter GetFilter() const = 0;

  /**
   * Gets the hierarchy limits.
   * @return  Returns the hierarchy limits.
   */
  std::vector<HierarchyLimits>& GetHierarchyLimits();

 protected:
  // Hierarchy limits.
  std::vector<HierarchyLimits> hierarchy_limits_;

  // Distance from the destination within which "not_thru" roads are
  // considered. All costing methods exclude such roads except when close
  // to the destination
  float not_thru_distance_;
};

typedef std::shared_ptr<DynamicCost> cost_ptr_t;

}
}

#endif  // VALHALLA_THOR_DYNAMICCOST_H_
