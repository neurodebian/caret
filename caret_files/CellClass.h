
#ifndef __CELL_CLASS_H__
#define __CELL_CLASS_H__

/// This holds cell class information
class CellClass {
   public:
      /// Constructor
      CellClass(const QString& nameIn) {
         name = nameIn;
         selected = true;
      }
      
      /// get the class name
      QString getName() const { return name; }
      
      /// get selected status
      bool getSelected() const { return selected; }
      
      /// set selected status
      void setSelected(const bool sel) { selected = sel; }
      
      /// comparison operator
      bool operator<(const CellClass& cc) const {
         return (name < cc.name);
      }
      
      /// equality operator
      bool operator==(const CellClass& cc) const {
         return (name == cc.name);
      }
      
   private:
      /// name of class
      QString name;
      
      /// class selected
      bool selected;
      
   friend class CellData;
   friend class CellBase;
   friend class CellFile;
   friend class CellProjection;
   friend class CellProjectionFile;
};

#endif // __CELL_CLASS_H__

