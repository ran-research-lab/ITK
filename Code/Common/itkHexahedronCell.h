/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkHexahedronCell.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#ifndef __itkHexahedronCell_h
#define __itkHexahedronCell_h

#include "itkCellInterface.h"
#include "itkCellBoundary.h"
#include "itkQuadrilateralCell.h"

namespace itk
{

/** \class HexahedronCell
 * HexahedronCell represents a hexahedron for a Mesh.
 *
 * The CellBoundary wrapper for this cell is HexahedronBoundary.
 *
 * Template parameters for HexahedronCell:
 *
 * TPixelType =
 *     The type associated with a point, cell, or boundary for use in storing
 *     its data.
 *
 * TCellType =
 *     Type information of mesh containing cell.
 */

template <
  typename TPixelType,
  typename TCellType
  >
class HexahedronCell: public CellInterface< TPixelType , TCellType >
{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef HexahedronCell      Self;
  
  /**
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;

  /** \typedef
   * Save some template parameter information.
   */
  typedef typename CellType::CoordRep         CoordRep;
  typedef typename CellType::PointIdentifier  PointIdentifier;
  enum { PointDimension = CellType::PointDimension };

  /**
   * The type of boundary for this hexahedron's vertices.
   */
  typedef VertexBoundary< TPixelType , TCellType >         Vertex;

  /**
   * The type of boundary for this hexahedron's edges.
   */
  typedef LineBoundary< TPixelType , TCellType >           Edge;

  /**
   * The type of boundary for this hexahedron's faces.
   */
  typedef QuadrilateralBoundary< TPixelType , TCellType >  Face;
  
  /** \enum
   * Hexahedron-specific topology numbers.
   */
  enum { NumberOfPoints   =  8,
         NumberOfVertices =  8,
         NumberOfEdges    = 12,
         NumberOfFaces    =  6,
         CellDimension    =  3 };

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);
  
  /**
   * Implement the standard CellInterface.
   */
  virtual int GetCellDimension(void);
  virtual CellFeatureCount GetNumberOfBoundaryFeatures(int dimension);
  virtual Cell::Pointer GetBoundaryFeature(int dimension, CellFeatureIdentifier);
  virtual void SetCellPoints(const PointIdentifier *ptList);
  virtual void SetCellPoints(const PointIdentifier* first,
			     const PointIdentifier* last);
  virtual void SetCellPoint(int localId, PointIdentifier);
  virtual PointIterator      PointIdsBegin(void);
  virtual PointConstIterator PointIdsBegin(void) const;
  virtual PointIterator      PointIdsEnd(void);
  virtual PointConstIterator PointIdsEnd(void) const; 

  /**
   * Hexahedron-specific interface.
   */
  
  virtual CellFeatureCount GetNumberOfVertices(void);
  virtual CellFeatureCount GetNumberOfEdges(void);
  virtual CellFeatureCount GetNumberOfFaces(void);
  virtual Vertex::Pointer GetCellVertex(CellFeatureIdentifier);
  virtual Edge::Pointer GetCellEdge(CellFeatureIdentifier);  
  virtual Face::Pointer GetCellFace(CellFeatureIdentifier);  

  /**
   * Standard part of every itk Object.
   */
  itkTypeMacro(HexahedronCell, CellInterface);

protected:
  /**
   * Store the number of points needed for a hexahedron.
   */
  PointIdentifier m_PointIds[NumberOfPoints];
  
  /**
   * Hexahedron topology data.
   */
  static const int m_Edges[12][2];
  static const int m_Faces[6][4];
};


/** \class HexahedronBoundary
 * Create a boundary-wrapped version of the HexahedronCell.
 */
template <typename TPixelType, typename TCellType>
class HexahedronBoundary:
  public CellBoundary< HexahedronCell< TPixelType , TCellType > >
{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef HexahedronBoundary  Self;

  /**
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;
  
  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);

  /**
   * Standard part of every itk Object.
   */
  itkTypeMacro(HexahedronBoundary, CellBoundary);
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHexahedronCell.txx"
#endif

#endif
