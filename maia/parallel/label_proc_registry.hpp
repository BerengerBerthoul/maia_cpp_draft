#pragma once

#include "tmp_cgns/cgns/cgns_registery.hpp"

/** 
 * label_proc_registry: Similar to a label_registry, with owning proc added
*/
class label_proc_registry {
  // Class invariant
  //   - procs()[i] is associated to ids()[i]
  public:
    label_proc_registry(label_registry label_reg, std::vector<int> owning_procs)
      : label_reg(std::move(label_reg))
      , owning_procs(std::move(owning_procs))
    {}

    int nb_entities() const {
      return label_reg.nb_entities();
    }
    auto ids() const -> const auto& {
      return label_reg.ids();
    }
    auto paths() const -> const auto& {
      return label_reg.entities();
    }
    auto procs() const -> const auto& {
      return owning_procs;
    }
  private:
    label_registry label_reg;
    std::vector<int> owning_procs;
};
