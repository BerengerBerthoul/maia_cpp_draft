## User profile

class parallel_tree:
  def __init__(self,dist_tree,part_tree):
    self.part_tree = part_tree
    self.dist_tree = dist_tree

  def get_part_tree(self):
    return self.part_tree
  def set_part_tree(self,part_tree):
    self.part_tree = part_tree

  def get_dist_tree(self):
    return self.dist_tree
  def set_dist_tree(self,dist_tree):
    return self.dist_tree = dist_tree


#def read_tree_with_FSDM_profile(filename):
#  dist_tree = load_dist_tree(filename)
#  part_tree = partition_tree(dist_tree) 
#
#  add_vtx_joins(part_tree)
#
#  add_owner_to_joins(part_tree)
#
#  reorder_with_non_owner_vtx_as_rind()


def read_tree_for_FSDM(filename):
  dist_tree = load_dist_tree(filename)
  part_tree = partition_tree(dist_tree,join_type='Vertex')

  assign_zone_owner_to_vtx_joins(part_tree)
  reorder_with_non_owner_vtx_as_rind(part_tree)

  return parallel_tree(dist_tree,part_tree)


def write_tree_from_FSDM(ptree,filename):
  dist_tree = load_dist_tree(filename)
  part_tree = partition_tree(dist_tree,join_type='Vertex')

  assign_zone_owner_to_vtx_joins(part_tree)
  reorder_with_non_owner_vtx_as_rind(part_tree)

  return part_tree


class fs_cgns_converter:
  def __init__(self):
    self.ptree = None

  def read(filename)
    self.ptree = read_tree_for_FSDM(filename)
    t = ptree.get_part_tree()

    return fs_cgns_adapter.view_as_fs_mesh(t)

  def write(
    t = fs_cgns_adapter.view_as_pytree(fsmesh) # or update(t,fsmesh)?

    if self.ptree in not None:
      self.ptree.set_part_tree(t)

      write_tree_from_FSDM(ptree,filename)




"""
FSDM tree: interleaved, 0-indexed...
"""
def read_FSDM_tree(filename):
  return read_tree_one_zone_by_proc()
  pass


def write_FSDM_tree(part_tree,filename):


