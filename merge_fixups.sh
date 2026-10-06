# 11. Same call, the human's typed parameter: cast at the call site.
sed -i -e 's|NSBXX_Tex_GetBlock1Length((struct ScaledField020b2e3c\*)val)|NSBXX_Tex_GetBlock1Length((NSBXXTex*)val)|' \
       -e 's|NSBXX_Tex_GetBlock4Length((unsigned char\*)val)|NSBXX_Tex_GetBlock4Length((NSBXXTex*)val)|' \
    src/Combat/Main/AccumulateHandleStats0201498c.cpp 2>/dev/null

# 12. Model3D::Restage*() are void, and these callers return the value. Going
#     through the class would need `m(); return 0;` -- an extra `mov r0,#0` that
#     stops the function matching -- so keep the mangled bridge.
for pair in "Field0x1cCall0207ecd4OrNull RestageTexturePalette _ZN7Model3D21RestageTexturePaletteEv Obj0207ecd4" \
            "Field0x1cCall0207ecf8OrNull RestageTextureImage   _ZN7Model3D19RestageTextureImageEv   Obj0207ecf8"; do
  set -- $pair
  f="src/Combat/Main/$1.cpp"
  [ -f "$f" ] || continue
  if grep -q -- "->$2()" "$f"; then
    sed -i -e "s|return ((Model3D\*)((struct $4\*)x))->$2();|return $3(x);|" \
           -e "1a extern \"C\" void* $3(void*);" "$f"
  fi
done
