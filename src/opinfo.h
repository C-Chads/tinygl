

ADD_OP(Color, 7, "%f %f %f %f %d %d %d")
ADD_OP(TexCoord, 4, "%f %f %f %f")
ADD_OP(EdgeFlag, 1, "%d")
ADD_OP(Normal, 3, "%f %f %f")

ADD_OP(Begin, 1, "%C")
ADD_OP(Vertex, 4, "%f %f %f %f")
ADD_OP(End, 0, "")

ADD_OP(EnableDisable, 2, "%C %d")

ADD_OP(MatrixMode, 1, "%C")
ADD_OP(LoadMatrix, 16, "")
ADD_OP(LoadIdentity, 0, "")
ADD_OP(MultMatrix, 16, "")
ADD_OP(PushMatrix, 0, "")
ADD_OP(PopMatrix, 0, "")
ADD_OP(Rotate, 4, "%f %f %f %f")
ADD_OP(Translate, 3, "%f %f %f")
ADD_OP(Scale, 3, "%f %f %f")

ADD_OP(Viewport, 4, "%d %d %d %d")
ADD_OP(Frustum, 6, "%f %f %f %f %f %f")

ADD_OP(Material, 6, "%C %C %f %f %f %f")
ADD_OP(ColorMaterial, 2, "%C %C")
ADD_OP(Light, 6, "%C %C %f %f %f %f")
ADD_OP(LightModel, 5, "%C %f %f %f %f")

ADD_OP(Clear, 1, "%d")
ADD_OP(ClearColor, 4, "%f %f %f %f")
ADD_OP(ClearDepth, 1, "%f")

ADD_OP(InitNames, 0, "")
ADD_OP(PushName, 1, "%d")
ADD_OP(PopName, 0, "")
ADD_OP(LoadName, 1, "%d")

ADD_OP(TexImage2D, 9, "%d %d %d  %d %d %d  %d %d %d")
ADD_OP(TexImage1D, 8, "%d %d  %d %d %d  %d %d %d")
ADD_OP(CopyTexImage2D, 8, "%d %d %d %d  %d %d %d %d")
ADD_OP(BindTexture, 2, "%C %d")




ADD_OP(ShadeModel, 1, "%C")
ADD_OP(CullFace, 1, "%C")
ADD_OP(FrontFace, 1, "%C")
ADD_OP(PolygonMode, 2, "%C %C")

ADD_OP(CallList, 1, "%d")


/* special opcodes */
ADD_OP(EndList, 0, "")
ADD_OP(NextBuffer, 1, "%p")

/* opengl 1.1 arrays */
ADD_OP(ArrayElement, 1, "%d")
ADD_OP(EnableClientState, 1, "%C")
ADD_OP(DisableClientState, 1, "%C")
ADD_OP(VertexPointer, 4, "%d %C %d %p")
ADD_OP(ColorPointer, 4, "%d %C %d %p")
ADD_OP(NormalPointer, 3, "%C %d %p")
ADD_OP(TexCoordPointer, 4, "%d %C %d %p")

/* opengl 1.1 polygon offset */
ADD_OP(PolygonOffset, 2, "%f %f")

/* blending */
ADD_OP(BlendEquation, 1, "%d")
ADD_OP(BlendFunc, 2, "%d %d")

/* point size */
ADD_OP(PointSize, 1, "%f")

/* raster position */
ADD_OP(RasterPos, 4, "%f %f %f %f")
ADD_OP(PixelZoom, 2, "%f %f")
/* Draw pixels*/
/* Width, Height, Data*/
ADD_OP(DrawPixels, 3, "%d %d %p")

/* Gek's Added Functions */
ADD_OP(PlotPixel, 2, "%d %d")
ADD_OP(TextSize, 1, "%d")
ADD_OP(SetEnableSpecular, 1, "%d")

#undef ADD_OP
